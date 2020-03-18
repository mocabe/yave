//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

#include <yave/rts/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/mat/mat.hpp>

namespace yave {

  namespace node {
    /// Transform
    struct Transform;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Transform);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Transform, modules::_std::tag);

  namespace modules::_std::core {
    /// get transform from demand
    struct GetCurrentTransform
      : Function<GetCurrentTransform, FrameDemand, FMat4>
    {
      return_type code() const
      {
        return make_object<FMat4>(eval_arg<0>()->transform);
      }
    };
  } // namespace modules::_std::core

} // namespace yave