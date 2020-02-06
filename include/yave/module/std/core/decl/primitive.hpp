//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>
#include <yave/rts/object_ptr.hpp>

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

  struct primitive_data_holder_object_value
  {
    primitive_data_holder_object_value(
      object_ptr<Object> d,
      object_ptr<Object> c)
      : data {std::move(d)}
      , ctor {std::move(c)}
    {
    }

    primitive_data_holder_object_value(
      const primitive_data_holder_object_value& other)
    {
      data = other.data.clone();
      ctor = other.ctor.clone();
    }

    object_ptr<Object> data; // data
    object_ptr<Object> ctor; // constructor function referring data

    [[nodiscard]] auto get_ctor() -> object_ptr<Object>
    {
      return ctor << data;
    }
  };

  using PrimitiveDataHolder = Box<primitive_data_holder_object_value>;

  template <class T>
  struct PrimitiveDataCtor : Function<PrimitiveDataCtor<T>, T, FrameDemand, T>
  {
    auto code() const -> typename PrimitiveDataCtor::return_type
    {
      return PrimitiveDataCtor::template eval_arg<0>();
    }
  };

  template <class T>
  struct node_declaration_traits<node::PrimitiveConstructor<T>>
  {
    static auto get_node_declaration() -> node_declaration
    {
      auto data = make_object<T>();
      auto ctor = make_object<PrimitiveDataCtor<T>>();
      return node_declaration(
        primitive_type_traits<T>::name,
        {"value"},
        {"value"},
        "Primitive constructor",
        {{0, make_object<PrimitiveDataHolder>(data, ctor)}});
    }
  };
} // namespace yave

YAVE_DECL_TYPE(
  yave::PrimitiveDataHolder,
  "8cfff49e-b86b-4cca-8d9d-34800003ff3b");