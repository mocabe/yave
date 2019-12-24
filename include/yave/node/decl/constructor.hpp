//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/primitive.hpp>
#include <yave/obj/string/string.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  namespace node {

    /// Primitive constructor node
    template <class T>
    struct PrimitiveConstructor;

    /* aliases */

    using Int8  = PrimitiveConstructor<yave::Int8>;
    using Int16 = PrimitiveConstructor<yave::Int16>;
    using Int32 = PrimitiveConstructor<yave::Int32>;
    using Int64 = PrimitiveConstructor<yave::Int64>;

    using UInt8  = PrimitiveConstructor<yave::UInt8>;
    using UInt16 = PrimitiveConstructor<yave::UInt16>;
    using UInt32 = PrimitiveConstructor<yave::UInt32>;
    using UInt64 = PrimitiveConstructor<yave::UInt64>;

    using Int   = PrimitiveConstructor<yave::Int32>;
    using Long  = PrimitiveConstructor<yave::Int64>;
    using UInt  = PrimitiveConstructor<yave::UInt32>;
    using ULong = PrimitiveConstructor<yave::UInt64>;

    using Float  = PrimitiveConstructor<yave::Float>;
    using Double = PrimitiveConstructor<yave::Double>;
    using Bool   = PrimitiveConstructor<yave::Bool>;
    using String = PrimitiveConstructor<yave::String>;

    // Keyframe values (KeframeInt and others are dummy)
    using KeyframeDataInt   = PrimitiveConstructor<yave::KeyframeDataInt>;
    using KeyframeDataFloat = PrimitiveConstructor<yave::KeyframeDataFloat>;
    using KeyframeDataBool  = PrimitiveConstructor<yave::KeyframeDataBool>;

  } // namespace node

  template <class T>
  struct node_declaration_traits<node::PrimitiveConstructor<T>>
  {
    static auto get_node_declaration() -> node_declaration
    {
      using value_type = typename T::value_type;
      return node_declaration(
        get_primitive_name(primitive_t {value_type {}}),
        {},
        {"value"},
        object_type<node_closure<T>>(),
        object_type<T>());
    }
  };

} // namespace yave