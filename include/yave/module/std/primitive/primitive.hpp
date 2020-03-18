//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/module/std/primitive/data_holder.hpp>

#define YAVE_DECL_PRIM_NODE(TYPE)                                          \
  namespace node {                                                         \
    using TYPE = DataTypeConstructor<yave::TYPE>;                          \
  }                                                                        \
  template <>                                                              \
  constexpr char node_declaration_traits<node::TYPE>::node_name[] = #TYPE; \
  extern template struct node_declaration_traits<node::TYPE>;              \
  extern template struct node_definition_traits<node::TYPE, modules::_std::tag>

#define YAVE_DEF_PRIM_NODE(TYPE)                       \
  template struct node_declaration_traits<node::TYPE>; \
  template struct node_definition_traits<node::TYPE, modules::_std::tag>

namespace yave {

  namespace node {

    /// Data constructor node
    template <class T>
    struct DataTypeConstructor;

  } // namespace node

  template <class T>
  struct node_declaration_traits<node::DataTypeConstructor<T>>
  {
    static auto get_node_declaration() -> node_declaration;
    static const char node_name[];
  };

  template <class T>
  struct node_definition_traits<
    node::DataTypeConstructor<T>,
    modules::_std::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>;
  };

  // Also need to add definitions in .cpp to property link with clang.
  YAVE_DECL_PRIM_NODE(Int);
  YAVE_DECL_PRIM_NODE(Float);
  YAVE_DECL_PRIM_NODE(Bool);
  YAVE_DECL_PRIM_NODE(String);

} // namespace yave