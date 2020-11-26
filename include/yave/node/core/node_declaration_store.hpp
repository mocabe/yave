//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_declaration.hpp>

#include <map>
#include <memory>
#include <list>
#include <variant>

namespace yave {

  /// declaratoin map
  class node_declaration_map
  {
    using map_t =
      std::map<std::string, std::shared_ptr<const node_declaration>>;

    map_t m_map;

  public:
    node_declaration_map()                            = default;
    node_declaration_map(const node_declaration_map&) = default;
    node_declaration_map(node_declaration_map&&)      = default;
    node_declaration_map& operator=(const node_declaration_map&) = default;
    node_declaration_map& operator=(node_declaration_map&&) = default;

  public:
    /// Add declaration.
    void add(const std::shared_ptr<const node_declaration>& decl);

    /// Remvoe declaration
    void remove(const std::string& full_name);

    /// Exists?
    [[nodiscard]] bool exists(const std::string& name) const;

    /// Find
    [[nodiscard]] auto find(const std::string& name) const
      -> std::shared_ptr<const node_declaration>;
    // size
    [[nodiscard]] auto size() const -> size_t;

    /// empty?
    [[nodiscard]] bool empty() const;

    /// Clear
    void clear();
  };

  /// tree of declarations
  class node_declaration_tree
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// opaque node type
    struct node;

  public:
    node_declaration_tree();
    ~node_declaration_tree() noexcept;

  public:
    /// get root node
    auto root() const -> const node*;
    /// get children
    auto children(const node* n) const -> std::vector<const node*>;
    /// get name
    auto name(const node* n) const -> std::string;
    /// get decl (if node is leaf)
    auto decl(const node* n) const -> std::shared_ptr<const node_declaration>;

  public:
    /// insert
    void add(const std::shared_ptr<const node_declaration>& pdecl);
    /// remove
    void remove(const std::shared_ptr<const node_declaration>& pdecl);
    /// find
    auto find(const std::shared_ptr<const node_declaration>& pdecl) -> node*;
    /// clear
    void clear();
  };

  /// list of declarations
  class node_declaration_list
  {
    std::vector<std::shared_ptr<const node_declaration>> m_list;

  public:
    node_declaration_list()                             = default;
    node_declaration_list(const node_declaration_list&) = default;
    node_declaration_list(node_declaration_list&&)      = default;
    node_declaration_list& operator=(const node_declaration_list&) = default;
    node_declaration_list& operator=(node_declaration_list&&) = default;

  public:
    // clang-format off
    auto  begin() const              { return m_list.begin(); }
    auto  begin()                    { return m_list.begin(); }
    auto  end() const                { return m_list.end(); }
    auto  end()                      { return m_list.end(); }
    auto& operator[](size_t i) const { return m_list[i]; }
    auto& operator[](size_t i)       { return m_list[i]; }
    auto  size() const               { return m_list.size(); }
    // clang-format on

  public:
    /// insert
    void add(const std::shared_ptr<const node_declaration>& pdecl);
    /// remove
    void remove(const std::shared_ptr<const node_declaration>& pdecl);
    /// clear
    void clear();
  };

  class node_declaration_store
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// Constructor
    node_declaration_store();
    /// Destructor
    ~node_declaration_store() noexcept;

    /// Copy ctor
    node_declaration_store(const node_declaration_store&) = delete;
    /// Copy assign op
    node_declaration_store& operator=(const node_declaration_store&) = delete;

    /// Move ctor
    node_declaration_store(node_declaration_store&&) noexcept;
    /// Move assign op
    node_declaration_store& operator=(node_declaration_store&&) noexcept;

    /// Add declaration
    /// \requires `decl` should not be interface node
    [[nodiscard]] void add(const node_declaration& decl);

    /// Add declarations
    /// \requires `decls` should not contain interface node
    [[nodiscard]] void add(const std::vector<node_declaration>& decls);

    /// Exists?
    [[nodiscard]] bool exists(const std::string& name) const;

    /// Find
    [[nodiscard]] auto find(const std::string& name) const
      -> std::shared_ptr<const node_declaration>;

    /// size
    [[nodiscard]] auto size() const -> size_t;

    /// get list
    [[nodiscard]] auto get_list() const -> const node_declaration_list&;

    /// get public declaration tree
    [[nodiscard]] auto get_pub_tree() const -> const node_declaration_tree&;

    /// get map
    [[nodiscard]] auto get_map() const -> const node_declaration_map&;

    /// Remove declaration
    void remove(const std::string& name);

    /// Remove declarations
    void remove(const std::vector<std::string>& names);

    /// clear
    void clear();

    /// empty?
    bool empty() const;
  };
} // namespace yave