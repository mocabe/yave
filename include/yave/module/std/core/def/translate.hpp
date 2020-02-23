//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/translate.hpp>
#include <yave/node/core/function.hpp>

#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/obj/mat/mat.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace yave {

  namespace modules::_std::core {

    class Translate_X;

    struct Translate : Function<
                         Translate,
                         node_closure<forall<Translate_X>>,
                         node_closure<Float>,
                         node_closure<Float>,
                         node_closure<Float>,
                         FrameDemand,
                         forall<Translate_X>>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<4>();
        auto x  = eval(arg<1>() << fd);
        auto y  = eval(arg<2>() << fd);
        auto z  = eval(arg<3>() << fd);
        auto m  = glm::translate(glm::mat4(1), glm::vec3(*x, *y, *z));
        return arg<0>() << make_object<FrameDemand>(fd->time, fd->matrix * m);
      }
    };
  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::Translate, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Translate>();

      return {node_definition(
        info.name(),
        0,
        make_object<InstanceGetterFunction<modules::_std::core::Translate>>(),
        info.description())};
    }
  };
} // namespace yave