//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_info.hpp>
#include <yave/node/core/bind_info.hpp>

namespace yave {

  /// Trait class for info getters
  template <class T>
  struct node_function_info_traits
  {
    // static node_info get_node_info(...);
    // static bind_info get_bind_info(...);
  };

  /// Get node_info of node funciton (if provided)
  template <class T, class... Args>
  node_info get_node_info(Args&&... args)
  {
    return node_function_info_traits<T>::get_node_info(
      std::forward<Args>(args)...);
  }

  /// Get bind_info of node function (if provided)
  template <class T, class... Args>
  bind_info get_bind_info(Args&&... args)
  {
    return node_function_info_traits<T>::get_bind_info(
      std::forward<Args>(args)...);
  }

} // namespace yave
