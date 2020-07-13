//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/transform/rotate.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/obj/mat/mat.hpp>

namespace yave {

  auto node_declaration_traits<node::Rotate>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Rotate",
      "/std/transform",
      "Rotate object around axis",
      {"target", "deg", "axis"},
      {"out"},
      {{1, make_data_type_holder<Float>()}});
  }

  auto node_declaration_traits<node::RotateX>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "RotateX",
      "/std/transform",
      "Rotate object around X axis",
      {"target", "deg"},
      {"out"},
      {{1, make_data_type_holder<Float>()}});
  }

  auto node_declaration_traits<node::RotateY>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "RotateY",
      "/std/transform",
      "Rotate object around Y axis",
      {"target", "deg"},
      {"out"},
      {{1, make_data_type_holder<Float>()}});
  }

  auto node_declaration_traits<node::RotateZ>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "RotateZ",
      "/std/transform",
      "Rotate object around Z axis",
      {"target", "deg"},
      {"out"},
      {{1, make_data_type_holder<Float>()}});
  }
} // namespace yave