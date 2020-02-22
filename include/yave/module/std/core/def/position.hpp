//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/position.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/vec/vec.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace modules::_std::core {

    /// FrameDemand -> Vec3
    struct PositionConstructor
      : NodeFunction<PositionConstructor, FrameDemand, FVec3>
    {
      return_type code() const
      {
        return make_object<FVec3>(eval_arg<0>()->position);
      }
    };

    /// Vec3 -> Vec3
    struct PositionConstructor2
      : NodeFunction<PositionConstructor2, FVec3, FVec3>
    {
      return_type code() const
      {
        return eval_arg<0>();
      }
    };

  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::Position, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Position>();

      auto def1 = node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::PositionConstructor>>(),
        "Internal: Construct Vec3 from FrameDemand object");

      auto def2 = node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::PositionConstructor2>>(),
        "Construct Vec3 fromt Vec3");

      return {std::move(def1), std::move(def2)};
    }
  };

} // namespace yave