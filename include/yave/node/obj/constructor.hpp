//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/primitive.hpp>

namespace yave {

  /// Primitive constructor node
  template <class T>
  struct PrimitiveConstructor;

  template <class T>
  struct node_info_traits<PrimitiveConstructor<T>>
  {
    static node_info get_node_info()
    {
      using value_type = typename T::value_type;
      auto name        = get_primitive_name(primitive_t {value_type {}});
      return node_info(name, {}, {"value"});
    }
  };

} // namespace yave