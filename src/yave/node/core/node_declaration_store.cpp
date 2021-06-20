//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_declaration_store.hpp>
#include <yave/core/log.hpp>

#include <variant>
#include <list>
#include <range/v3/view.hpp>
#include <range/v3/algorithm.hpp>

YAVE_DECL_LOCAL_LOGGER(node_declaration_store)

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

  ////////////////////////////////////////
  // node_declaration_map

  void node_declaration_map::add(
    const std::shared_ptr<const node_declaration>& pdecl)
  {
    auto [it, succ] = m_map.emplace(pdecl->full_name(), pdecl);

    if (!succ) {
      auto& name = it->second->full_name();
      auto& iss  = it->second->input_sockets();
      auto& oss  = it->second->output_sockets();

      if (iss != pdecl->input_sockets() || oss != pdecl->output_sockets())
        log_warning(
          "Declaration of {} conflicting. Existing declaration will be used.",
          pdecl->full_name());
    }
  }

  void node_declaration_map::remove(const std::string& full_name)
  {
    auto iter = m_map.find(full_name);

    if (iter == m_map.end())
      return;

    log_info("Removed declaration: {}", full_name);
    m_map.erase(iter);
  }

  bool node_declaration_map::exists(const std::string& name) const
  {
    auto iter = m_map.find(name);
    return iter != m_map.end();
  }

  auto node_declaration_map::find(const std::string& name) const
    -> std::shared_ptr<const node_declaration>
  {
    auto iter = m_map.find(name);

    if (iter == m_map.end())
      return nullptr;

    return iter->second;
  }

  auto node_declaration_map::size() const -> size_t
  {
    return m_map.size();
  }

  bool node_declaration_map::empty() const
  {
    return m_map.empty();
  }

  void node_declaration_map::clear()
  {
    m_map.clear();
  }

  ////////////////////////////////////////
  // node_declaration_tree

  struct node_declaration_tree::node
  {
    /// parent node
    node* parent = nullptr;
    /// name
    std::string name = "";
    /// decl (nullable)
    std::shared_ptr<const node_declaration> pdecl = nullptr;
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

    void add(const std::shared_ptr<const node_declaration>& pdecl)
    {
      assert(pdecl);
      auto& decl = *pdecl;

      auto names = split_path_name(decl.full_name());

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
            return;
          }
          continue;
        }

        // already exists
        if (idx == names.size() - 1) {
          return;
        }

        // set next
        n = &*it;
      }
    }

    void remove(const std::shared_ptr<const node_declaration>& pdecl)
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

    auto find(const std::shared_ptr<const node_declaration>& pdecl) -> node*
    {
      assert(pdecl);
      auto names = split_path_name(pdecl->full_name());

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

    void clear()
    {
      tree = {};
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
    -> std::shared_ptr<const node_declaration>
  {
    return m_pimpl->decl(n);
  }

  void node_declaration_tree::add(
    const std::shared_ptr<const node_declaration>& pdecl)
  {
    m_pimpl->add(pdecl);
  }

  void node_declaration_tree::remove(
    const std::shared_ptr<const node_declaration>& pdecl)
  {
    m_pimpl->remove(pdecl);
  }

  auto node_declaration_tree::find(
    const std::shared_ptr<const node_declaration>& pdecl) -> node*
  {
    return m_pimpl->find(pdecl);
  }

  void node_declaration_tree::clear()
  {
    m_pimpl->clear();
  }

  ////////////////////////////////////////
  // node_declaration_list

  void node_declaration_list::add(
    const std::shared_ptr<const node_declaration>& pdecl)
  {
    auto lb = std::lower_bound(
      m_list.begin(), m_list.end(), pdecl, [](auto& l, auto& r) {
        return l->full_name() < r->full_name();
      });

    assert(lb == m_list.end() || *lb != pdecl);
    m_list.insert(lb, pdecl);
  }

  void node_declaration_list::remove(
    const std::shared_ptr<const node_declaration>& pdecl)
  {
    auto lb = std::lower_bound(
      m_list.begin(), m_list.end(), pdecl, [](auto& l, auto& r) {
        return l->full_name() < r->full_name();
      });

    if (lb != m_list.end() && *lb == pdecl)
      m_list.erase(lb);
  }

  void node_declaration_list::clear()
  {
    m_list.clear();
  }

  ////////////////////////////////////////
  // node_declaration_store

  class node_declaration_store::impl
  {
    /// decl map
    node_declaration_map m_map;
    /// decl tree
    node_declaration_tree m_pub_tree;
    /// decl list
    node_declaration_list m_list;

  public:
    impl()
    {
    }

  public:
    void add(const node_declaration& decl)
    {
      auto pdecl = std::make_shared<node_declaration>(decl);
      m_map.add(pdecl);
      m_pub_tree.add(pdecl);
      m_list.add(pdecl);
    }

    void add(const std::vector<node_declaration>& decls)
    {
      for (auto&& decl : decls)
        add(decl);
    }

    void remove(const std::string& name)
    {
      if (auto pdecl = m_map.find(name)) {
        m_list.remove(pdecl);
        m_pub_tree.remove(pdecl);
        m_map.remove(name);
      }
    }

    void remove(const std::vector<std::string>& names)
    {
      for (auto&& name : names)
        remove(name);
    }

    bool exists(const std::string& name) const
    {
      return m_map.exists(name);
    }

    auto find(const std::string& name) const
      -> std::shared_ptr<const node_declaration>
    {
      return m_map.find(name);
    }

    auto& get_map() const
    {
      return m_map;
    }

    auto& get_pub_tree() const
    {
      return m_pub_tree;
    }

    auto& get_list() const
    {
      return m_list;
    }

    auto size() const -> size_t
    {
      return m_map.size();
    }

    bool empty() const
    {
      return m_map.empty();
    }

    void clear()
    {
      m_map.clear();
      m_pub_tree.clear();
      m_list.clear();
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

  void node_declaration_store::add(const node_declaration& decl)
  {
    return m_pimpl->add(decl);
  }

  void node_declaration_store::add(const std::vector<node_declaration>& decls)
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

  auto node_declaration_store::get_list() const -> const node_declaration_list&
  {
    return m_pimpl->get_list();
  }

  auto node_declaration_store::get_pub_tree() const
    -> const node_declaration_tree&
  {
    return m_pimpl->get_pub_tree();
  }

  auto node_declaration_store::get_map() const -> const node_declaration_map&
  {
    return m_pimpl->get_map();
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