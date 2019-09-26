//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/obj/frame.hpp>
#include <yave/rts/rts.hpp>
#include <yave/obj/frame/frame.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace backend::default_render {

    struct FrameConstructor : Function<FrameConstructor, Frame, Frame>
    {
      return_type code() const
      {
        // reflect frame value
        return eval_arg<0>();
      }
    };

  } // namespace backend::default_render

  template <>
  struct bind_info_traits<node::Frame, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      auto info = get_node_info<node::Frame>();
      return bind_info(
        info.name(),
        info.input_sockets(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<backend::default_render::FrameConstructor>>(),
        info.name());
    }
  };

} // namespace yave