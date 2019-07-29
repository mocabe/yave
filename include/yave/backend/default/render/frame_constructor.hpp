//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/frame.hpp>

#include <yave/core/rts.hpp>
#include <yave/data/obj/frame.hpp>
#include <yave/backend/default/system/backend_tag.hpp>

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
  struct bind_info_traits<FrameConstructor, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      return bind_info(
        "Frame",
        {},
        "value",
        make_object<backend::default_render::FrameConstructor>(),
        "FrameConstructor");
    }
  };

} // namespace yave