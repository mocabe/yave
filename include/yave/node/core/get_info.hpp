//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/node_definition.hpp>

namespace yave {

  template <class Tag>
  struct node_declaration_traits
  {
    /// Specialize this function to provide declaration of node.
    static auto get_node_declaration() -> node_declaration;
  };

  template <class Tag, class BackendTag>
  struct node_definition_traits
  {
    /// For overloaded functions, return list of definitions.
    static auto get_node_definitions() -> std::vector<node_definition>;
  };

  /// Get node declaration
  template <class Tag>
  [[nodiscard]] auto get_node_declaration() -> node_declaration
  {
    return node_declaration_traits<Tag>::get_node_declaration();
  }

  /// Get node definitions
  template <class Tag, class BackendTag, class... Args>
  [[nodiscard]] auto get_node_definitions(Args&&... args)
    -> std::vector<node_definition>
  {
    return node_definition_traits<Tag, BackendTag>::get_node_definitions(
      std::forward<Args>(args)...);
  }

} // namespace yave
