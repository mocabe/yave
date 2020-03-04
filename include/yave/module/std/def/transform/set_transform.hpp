//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/config.hpp>
#include <yave/module/std/decl/transform/set_transform.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/mat/mat.hpp>

namespace yave {

  namespace modules::_std::geometry {

    class SetTransform_X;

    struct SetTransform : Function<
                            SetTransform,
                            node_closure<forall<SetTransform_X>>,
                            node_closure<FMat4>,
                            FrameDemand,
                            forall<SetTransform_X>>
    {
      return_type code() const
      {
        auto fd = eval_arg<2>();
        auto m  = eval(arg<1>() << fd);
        return arg<0>() << make_object<FrameDemand>(fd->time, *m);
      }
    };
  } // namespace modules::_std::geometry

  template <>
  struct node_definition_traits<node::SetTransform, modules::_std::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::SetTransform>();

      return {node_definition(
        info.name(),
        0,
        make_object<modules::_std::geometry::SetTransform>(),
        info.description())};
    }
  };
} // namespace yave