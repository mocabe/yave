//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/node_definition.hpp>
#include <yave/node/core/node_info.hpp>
#include <yave/node/core/bind_info.hpp>

namespace yave {

  /// trait class to define node info
  template <class Tag>
  struct node_info_traits
  {
    /// Specialize this function to provide info of node.
    static auto get_node_info() -> node_info;

    /// Specialize this function to provide type of node.
    /// \note Return type class for overloadable nodes.
    static auto get_node_type() -> object_ptr<const Type>;
  };

  /// trant class to define backend binding
  template <class T, class BackendTag>
  struct bind_info_traits
  {
    /// Specialize this function to provide bind of node.
    static auto get_bind_info(...) -> bind_info;
  };

  /// Get node info
  template <class T>
  [[nodiscard]] auto get_node_info() -> node_info
  {
    return node_info_traits<T>::get_node_info();
  }

  /// Get node type
  template <class T>
  [[nodiscard]] auto get_node_type() -> object_ptr<const Type>
  {
    return node_info_traits<T>::get_node_type();
  }

  /// Get bind info
  template <class T, class BackendTag, class... Args>
  [[nodiscard]] auto get_bind_info(Args&&... args) -> bind_info
  {
    auto info = bind_info_traits<T, BackendTag>::get_bind_info(
      std::forward<Args>(args)...);

    {
      auto ni = get_node_info<T>();
      assert(info.name() == ni.name());
    }
    return info;
  }

  template <class Tag>
  struct node_declaration_traits
  {
    /// Specialize this function to provide declaration of node.
    static auto get_node_declaration() -> node_declaration;
  };

  template <class Tag, class BackendTag>
  struct node_definition_traits
  {
    /// Specialize this function to provide definition of node.
    static auto get_node_definition() -> node_definition;
  };

  /// Get node declaration
  template <class Tag>
  [[nodiscard]] auto get_node_declaration() -> node_declaration
  {
    return node_declaration_traits<Tag>::get_node_declaration();
  }

  /// Get node definition
  template <class Tag, class BackendTag, class... Args>
  [[nodiscard]] auto get_node_definition(Args&&... args) -> node_definition
  {
    return node_definition_traits<Tag, BackendTag>::get_node_defintion(
      std::forward<Args>(args)...);
  }

} // namespace yave
