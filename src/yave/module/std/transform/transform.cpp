//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/transform/transform.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/mat/mat.hpp>

namespace yave {

  auto node_declaration_traits<node::Transform>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Transform",
      "/std/transform",
      "Make or get current transform matrix",
      {"value"},
      {"value"},
      {{0, make_object<modules::_std::core::GetCurrentTransform>()}});
  }

  namespace modules::_std::geometry {
    /// FMat4 -> FMat4
    struct TransformConstructor
      : NodeFunction<TransformConstructor, FMat4, FMat4>
    {
      return_type code() const
      {
        return arg<0>();
      }
    };
  } // namespace modules::_std::geometry

  auto node_definition_traits<node::Transform, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Transform>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::TransformConstructor>(),
      info.description())};
  }
} // namespace yave