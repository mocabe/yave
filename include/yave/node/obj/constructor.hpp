//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/primitive.hpp>
#include <yave/obj/string/string.hpp>

namespace yave {

  namespace node {

    /// Primitive constructor node
    template <class T>
    struct PrimitiveConstructor;

    /* aliases */

    using Int8  = PrimitiveConstructor<Int8>;
    using Int16 = PrimitiveConstructor<Int16>;
    using Int32 = PrimitiveConstructor<Int32>;
    using Int64 = PrimitiveConstructor<Int64>;

    using UInt8  = PrimitiveConstructor<UInt8>;
    using UInt16 = PrimitiveConstructor<UInt16>;
    using UInt32 = PrimitiveConstructor<UInt32>;
    using UInt64 = PrimitiveConstructor<UInt64>;

    using Int   = PrimitiveConstructor<Int32>;
    using Long  = PrimitiveConstructor<Int64>;
    using UInt  = PrimitiveConstructor<UInt32>;
    using ULong = PrimitiveConstructor<UInt64>;

    using Float  = PrimitiveConstructor<Float>;
    using Double = PrimitiveConstructor<Double>;
    using Bool   = PrimitiveConstructor<Bool>;
    using String = PrimitiveConstructor<String>;

    // Keyframe* have special definitions, so don't add them.

  } // namespace node

  template <class T>
  struct node_info_traits<node::PrimitiveConstructor<T>>
  {
    static node_info get_node_info()
    {
      using value_type = typename T::value_type;
      auto name        = get_primitive_name(primitive_t {value_type {}});
      return node_info(name, {}, {"value"}, /*.is_prim=*/true);
    }
  };

} // namespace yave