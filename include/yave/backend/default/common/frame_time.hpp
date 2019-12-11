//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/decl/frame_time.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace backends::default_common {

    /// FrameDemand -> FrameTime
    struct FrameTimeConstructor
      : Function<FrameTimeConstructor, FrameDemand, FrameTime>
    {
      return_type code() const
      {
        return make_object<FrameTime>(eval_arg<0>()->time);
      }
    };

  } // namespace backends::default_common

  template <>
  struct node_definition_traits<node::FrameTime, backend_tags::default_common>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::FrameTime>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<InstanceGetterFunction<
          backends::default_common::FrameTimeConstructor>>(),
        info.name())};
    }
  };

} // namespace yave