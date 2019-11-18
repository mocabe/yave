//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

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

} // namespace yave
