//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_declaration_store.hpp>
#include <yave/support/log.hpp>

#include <variant>
#include <list>
#include <range/v3/view.hpp>
#include <range/v3/algorithm.hpp>

YAVE_DECL_G_LOGGER(node_declaration_store)

namespace yave {

  namespace {

    namespace rs = ranges;
    namespace rv = ranges::views;

    auto split_path_name(const std::string& path)
    {
      return path                                                             //
             | rv::split('.')                                                 //
             | rv::drop_while([](auto&& s) { return s.empty(); })             //
             | rv::transform([](auto&& r) { return rs::to<std::string>(r); }) //
             | rs::to_vector;
    }
  } // namespace

  struct node_declaration_tree::node
  {
    /// parent node
    node* parent = nullptr;
    /// name
    std::string name = "";
    /// decl (nullable)
    std::shared_ptr<node_declaration> pdecl = nullptr;
    /// children
    std::list<node> children = {};

  public:
    bool is_leaf() const
    {
      return children.empty();
    }
  };

  class node_declaration_tree::impl
  {
    node tree;

  public:
    impl() = default;

    auto* root() const
    {
      return &tree;
    }

    auto children(const node* n) const -> std::vector<const node*>
    {
      assert(n);
      return n->children                                       //
             | rv::transform([](const auto& c) { return &c; }) //
             | rs::to_vector;
    }

    auto name(const node* n) const
    {
      assert(n);
      return n->name;
    }

    auto decl(const node* n) const
    {
      assert(n);
      return n->pdecl;
    }

  public:
    auto insert(const std::shared_ptr<node_declaration>& pdecl) -> node*
    {
      assert(pdecl);
      auto& decl = *pdecl;

      auto names = split_path_name(full_name_of(decl));

      node* n = &tree;

      for (auto&& [idx, name] : names | rv::enumerate) {

        auto it = rs::find_if(
          n->children, [name = name](auto& c) { return c.name == name; });

        if (it == n->children.end()) {

          // insert new node
          auto newit = n->children.insert(
            rs::find_if_not(
              n->children, [name = name](auto&& c) { return c.name < name; }),
            node {.parent = n, .name = name, .pdecl = nullptr, .children = {}});

          // set next
          n = &*newit;

          // leaf
          if (idx == names.size() - 1) {
            n->pdecl = pdecl;
            return n;
          }
          continue;
        }

        // already exists
        if (idx == names.size() - 1) {
          return nullptr;
        }

        // set next
        n = &*it;
      }
      return nullptr;
    }

    void remove(const std::shared_ptr<node_declaration>& pdecl)
    {
      assert(pdecl);

      /// find from path
      auto n = find(pdecl);

      if (!n)
        return;

      assert(pdecl == n->pdecl);

      auto it =
        rs::find_if(n->parent->children, [&](auto& c) { return n == &c; });

      n->parent->children.erase(it);
    }

    auto find(const std::shared_ptr<node_declaration>& pdecl) -> node*
    {
      assert(pdecl);
      auto names = split_path_name(full_name_of(*pdecl));

      node* n = &tree;

      for (auto&& name : names) {

        auto it =
          rs::find_if(n->children, [&](auto&& c) { return c.name == name; });

        if (it == n->children.end())
          return nullptr;

        n = &*it;
      }

      if (n->pdecl)
        return n;

      return nullptr;
    }
  };

  node_declaration_tree::node_declaration_tree()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  node_declaration_tree::~node_declaration_tree() noexcept = default;

  auto node_declaration_tree::root() const -> const node*
  {
    return m_pimpl->root();
  }

  auto node_declaration_tree::children(const node* n) const
    -> std::vector<const node*>
  {
    return m_pimpl->children(n);
  }

  auto node_declaration_tree::name(const node* n) const -> std::string
  {
    return m_pimpl->name(n);
  }

  auto node_declaration_tree::decl(const node* n) const
    -> std::shared_ptr<node_declaration>
  {
    return m_pimpl->decl(n);
  }

  auto node_declaration_tree::insert(
    const std::shared_ptr<node_declaration>& pdecl) -> node*
  {
    return m_pimpl->insert(pdecl);
  }

  void node_declaration_tree::remove(
    const std::shared_ptr<node_declaration>& pdecl)
  {
    m_pimpl->remove(pdecl);
  }

  auto node_declaration_tree::find(
    const std::shared_ptr<node_declaration>& pdecl) -> node*
  {
    return m_pimpl->find(pdecl);
  }

  class node_declaration_store::impl
  {
    /// decl map
    std::map<std::string, std::shared_ptr<node_declaration>> m_map = {};
    /// decl tree
    node_declaration_tree m_tree;

  public:
    impl()
    {
      init_logger();
    }

  public:
    bool add(const node_declaration& decl)
    {
      auto [it, succ] = m_map.emplace(
        full_name_of(decl), std::make_shared<node_declaration>(decl));

      if (succ) {
        Info(g_logger, "Added new declaration: {}", full_name_of(decl));
        m_tree.insert(it->second);
        return true;
      }

      auto& name = full_name_of(*it->second);
      auto& iss  = input_sockets_of(*it->second);
      auto& oss  = output_sockets_of(*it->second);

      // validate duplication
      if (
        name == full_name_of(decl) &&    //
        iss == input_sockets_of(decl) && //
        oss == output_sockets_of(decl)) {
        Info(
          g_logger,
          "Node declaration {} already exists, ignored.",
          full_name_of(decl));
        return true;
      }

      Error(g_logger, "Failed to add declaration: {}", full_name_of(decl));
      return false;
    }

    bool add(const std::vector<node_declaration>& decls)
    {
      std::vector<std::string> added;

      for (auto&& decl : decls) {
        if (add(decl)) {
          added.push_back(full_name_of(decl));
        } else {
          for (auto&& name : added) {
            remove(name);
          }
          return false;
        }
      }
      return true;
    }

    bool exists(const std::string& name) const
    {
      auto iter = m_map.find(name);
      return iter != m_map.end();
    }

    auto find(const std::string& name) const
      -> std::shared_ptr<const node_declaration>
    {
      auto iter = m_map.find(name);

      if (iter == m_map.end())
        return nullptr;

      return iter->second;
    }

    auto size() const -> size_t
    {
      return m_map.size();
    }

    auto enumerate() const -> std::vector<std::shared_ptr<node_declaration>>
    {
      std::vector<std::shared_ptr<node_declaration>> ret;
      for (auto&& [key, decl] : m_map) {
        (void)key;
        ret.push_back(decl);
      }
      return ret;
    }

    auto& get_tree() const
    {
      return m_tree;
    }

    void remove(const std::string& name)
    {
      auto iter = m_map.find(name);

      if (iter == m_map.end())
        return;

      Info(g_logger, "Removed declaration: {}", name);

      m_tree.remove(iter->second);
      m_map.erase(iter);
    }

    void remove(const std::vector<std::string>& names)
    {
      for (auto&& name : names)
        remove(name);
    }

    void clear()
    {
      m_map.clear();
    }

    bool empty() const
    {
      return m_map.empty();
    }
  };

  node_declaration_store::node_declaration_store()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  node_declaration_store::~node_declaration_store() noexcept = default;

  node_declaration_store::node_declaration_store(
    node_declaration_store&& other) noexcept
    : m_pimpl {std::move(other.m_pimpl)}
  {
  }

  node_declaration_store& node_declaration_store::operator=(
    node_declaration_store&& other) noexcept
  {
    std::swap(m_pimpl, other.m_pimpl);
    return *this;
  }

  bool node_declaration_store::add(const node_declaration& decl)
  {
    return m_pimpl->add(decl);
  }

  bool node_declaration_store::add(const std::vector<node_declaration>& decls)
  {
    return m_pimpl->add(decls);
  }

  bool node_declaration_store::exists(const std::string& name) const
  {
    return m_pimpl->exists(name);
  }

  auto node_declaration_store::find(const std::string& name) const
    -> std::shared_ptr<const node_declaration>
  {
    return m_pimpl->find(name);
  }

  auto node_declaration_store::size() const -> size_t
  {
    return m_pimpl->size();
  }

  auto node_declaration_store::enumerate() const
    -> std::vector<std::shared_ptr<node_declaration>>
  {
    return m_pimpl->enumerate();
  }

  auto node_declaration_store::get_tree() const -> const node_declaration_tree&
  {
    return m_pimpl->get_tree();
  }

  void node_declaration_store::remove(const std::string& name)
  {
    m_pimpl->remove(name);
  }

  void node_declaration_store::remove(const std::vector<std::string>& names)
  {
    m_pimpl->remove(names);
  }

  void node_declaration_store::clear()
  {
    m_pimpl->clear();
  }

  bool node_declaration_store::empty() const
  {
    return m_pimpl->empty();
  }
} // namespace yave