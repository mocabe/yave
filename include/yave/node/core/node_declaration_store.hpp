//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_declaration.hpp>

#include <map>
#include <memory>

namespace yave {

  class node_declaration_store
  {
    /// map type
    using map_type = std::map<std::string, std::shared_ptr<node_declaration>>;

  public:
    /// Constructor
    node_declaration_store();

    /// Copy ctor
    node_declaration_store(const node_declaration_store&);
    /// Move ctor
    node_declaration_store(node_declaration_store&&) noexcept;

    /// Copy assign op
    node_declaration_store& operator=(const node_declaration_store&);
    /// Move assign op
    node_declaration_store& operator=(node_declaration_store&&) noexcept;

    /// Add declaration
    [[nodiscard]] bool add(const node_declaration& decl);

    /// Add declarations
    [[nodiscard]] bool add(const std::vector<node_declaration>& decls);

    /// Exists?
    [[nodiscard]] bool exists(const std::string& name) const;

    /// Find
    [[nodiscard]] auto find(const std::string& name) const
      -> std::shared_ptr<node_declaration>;

    /// Remove declaration
    void remove(const std::string& name);

    /// Remove declarations
    void remove(const std::vector<std::string>& names);

    /// clear
    void clear();

  private:
    /// map
    map_type m_map;
  };
}