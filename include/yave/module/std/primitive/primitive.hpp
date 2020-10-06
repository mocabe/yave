//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/obj/primitive/primitive.hpp>

#define YAVE_DECL_PRIM_NODE(TYPE, MODULE)                                  \
  namespace node {                                                         \
    using TYPE = PrimitiveCtor<yave::TYPE>;                                \
  }                                                                        \
  template <>                                                              \
  constexpr char node_declaration_traits<node::TYPE>::node_name[] = #TYPE; \
  template <>                                                              \
  constexpr char node_declaration_traits<node::TYPE>::module[] = #MODULE;  \
  extern template struct node_declaration_traits<node::TYPE>;              \
  extern template struct node_definition_traits<node::TYPE, modules::_std::tag>

#define YAVE_DEF_PRIM_NODE(TYPE)                       \
  template struct node_declaration_traits<node::TYPE>; \
  template struct node_definition_traits<node::TYPE, modules::_std::tag>

namespace yave {

  namespace node {

    /// Data constructor node
    template <class T>
    struct PrimitiveCtor;

  } // namespace node

  template <class T>
  struct node_declaration_traits<node::PrimitiveCtor<T>>
  {
    static auto get_node_declaration() -> node_declaration;
    static const char node_name[];
    static const char module[];
  };

  template <class T>
  struct node_definition_traits<node::PrimitiveCtor<T>, modules::_std::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>;
  };

} // namespace yave