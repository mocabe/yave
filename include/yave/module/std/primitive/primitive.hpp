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


  using BoolDataProperty = ValueDataTypeProperty<Bool>;
  YAVE_DECL_DATA_TYPE_PROPERTY(Bool, Bool, BoolDataProperty);

  using StringDataProperty = ValueDataTypeProperty<String>;
  YAVE_DECL_DATA_TYPE_PROPERTY(String, String, StringDataProperty);

  using FloatDataProperty = NumericDataTypeProperty<Float>;
  YAVE_DECL_DATA_TYPE_PROPERTY(Float, Float, FloatDataProperty);

  using IntDataProperty = NumericDataTypeProperty<Int>;
  YAVE_DECL_DATA_TYPE_PROPERTY(Int, Int, IntDataProperty);

} // namespace yave

YAVE_DECL_TYPE(yave::IntDataProperty, "d60f1ac7-5a57-4037-9d78-c9805cbe5407");
YAVE_DECL_TYPE(yave::FloatDataProperty, "621188d3-d162-4778-aedd-3be0a4745c3e");
YAVE_DECL_TYPE(yave::BoolDataProperty, "95a0e534-273b-4911-957d-0902f8d769a4");
YAVE_DECL_TYPE(yave::StringDataProperty, "6d7e77a5-3cd3-4bd5-bd18-e9b1a02cb494");