//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>

#define YAVE_DECL_PRIM_NODE_DECL(TYPE, NAME)       \
  namespace node {                                 \
    using TYPE = PrimitiveConstructor<yave::TYPE>; \
  }                                                \
  template <>                                      \
  struct primitive_type_traits<yave::TYPE>         \
  {                                                \
    static constexpr const char name[] = #NAME;    \
  }

namespace yave {

  namespace node {

    /// Primitive constructor node
    template <class T>
    struct PrimitiveConstructor;

  } // namespace node

  /// Primitive traits
  template <class T>
  struct primitive_type_traits
  {
  };

  YAVE_DECL_PRIM_NODE_DECL(Int, Int);
  YAVE_DECL_PRIM_NODE_DECL(Float, Float);
  YAVE_DECL_PRIM_NODE_DECL(Bool, Bool);
  YAVE_DECL_PRIM_NODE_DECL(String, String);

  template <class T>
  struct node_declaration_traits<node::PrimitiveConstructor<T>>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        primitive_type_traits<T>::name,
        {},
        {"value"},
        object_type<node_closure<T>>(),
        make_object<T>());
    }
  };
} // namespace yave