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

  /// tree of declarations.
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
    auto decl(const node* n) const -> std::shared_ptr<node_declaration>;

  public:
    /// insert new node
    auto insert(const std::shared_ptr<node_declaration>& pdecl) -> node*;
    /// remove node
    void remove(const std::shared_ptr<node_declaration>& pdecl);
    /// find node
    auto find(const std::string& path) -> node*;
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
    [[nodiscard]] bool add(const node_declaration& decl);

    /// Add declarations
    /// \requires `decls` should not contain interface node
    [[nodiscard]] bool add(const std::vector<node_declaration>& decls);

    /// Exists?
    [[nodiscard]] bool exists(const std::string& name) const;

    /// Find
    [[nodiscard]] auto find(const std::string& name) const
      -> std::shared_ptr<const node_declaration>;

    /// size
    [[nodiscard]] auto size() const -> size_t;

    /// enumerate
    [[nodiscard]] auto enumerate() const
      -> std::vector<std::shared_ptr<node_declaration>>;

    /// get tree
    [[nodiscard]] auto get_tree() const -> const node_declaration_tree&;

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