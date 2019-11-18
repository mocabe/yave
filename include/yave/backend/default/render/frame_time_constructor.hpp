//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/class/frame_time.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace backend::default_render {

    /// Time -> Time
    using FrameTimeConstructor = Id;

  } // namespace backend::default_render

  template <>
  struct bind_info_traits<node::FrameTime, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      auto info = get_node_info<node::FrameTime>();
      return bind_info(
        info.name(),
        info.input_sockets(),
        info.output_sockets()[0],
        make_object<InstanceGetterFunction<
          backend::default_render::FrameTimeConstructor>>(),
        info.name());
    }
  };

} // namespace yave