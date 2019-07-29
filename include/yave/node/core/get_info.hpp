//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_info.hpp>
#include <yave/node/core/bind_info.hpp>

namespace yave {

  /// trait class to define node info
  template <class T>
  struct node_info_traits
  {
    // static node_info get_node_info(...);
  };

  /// trant class to define backend binding
  template <class T, class BackendTag>
  struct bind_info_traits
  {
    // static bind_info get_bind_info(...);
  };

  template <class T>
  node_info get_node_info()
  {
    return node_info_traits<T>::get_node_info();
  }

  template <class T, class BackendTag, class... Args>
  bind_info get_bind_info(Args&&... args)
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
