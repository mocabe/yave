//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/module/std/primitive/data_holder.hpp>

#define YAVE_DECL_PRIM_NODE_DECL(TYPE)            \
  namespace node {                                \
    using TYPE = DataTypeConstructor<yave::TYPE>; \
  }                                               \
  template <>                                     \
  struct data_type_traits<yave::TYPE>             \
  {                                               \
    static constexpr const char name[] = #TYPE;   \
  };                                              \
  template struct node_declaration_traits<        \
    node::DataTypeConstructor<yave::TYPE>>;       \
  template struct node_definition_traits<         \
    node::DataTypeConstructor<yave::TYPE>,        \
    modules::_std::tag>

namespace yave {

  namespace node {

    /// Data constructor node
    template <class T>
    struct DataTypeConstructor;

  } // namespace node

  /// Primitive traits
  template <class T>
  struct data_type_traits
  {
  };

  template <class T>
  struct node_declaration_traits<node::DataTypeConstructor<T>>
  {
    static auto get_node_declaration() -> node_declaration;
  };

  template <class T>
  struct node_definition_traits<
    node::DataTypeConstructor<T>,
    modules::_std::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>;
  };

  YAVE_DECL_PRIM_NODE_DECL(Int);
  YAVE_DECL_PRIM_NODE_DECL(Float);
  YAVE_DECL_PRIM_NODE_DECL(Bool);
  YAVE_DECL_PRIM_NODE_DECL(String);
} // namespace yave