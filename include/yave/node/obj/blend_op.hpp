//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/function.hpp>
#include <yave/node/obj/compositor.hpp>

namespace yave {

  struct BlendOpSrc
    : NodeFunction<BlendOpSrc, FrameBuffer, FrameBuffer, FrameBuffer>
  {
    return_type code()
    {
      return arg<0>();
    }
  };

  template <>
  struct node_function_info_traits<BlendOpSrc>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpSrc", {"src", "dst"}, {"out"});
    }

    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpSrc",
        {"src", "dst"},
        "out",
        make_object<InstanceGetterFunction<BlendOpSrc>>(),
        "BlendOpSrc");
    }
  };

  struct BlendOpDst
    : NodeFunction<BlendOpDst, FrameBuffer, FrameBuffer, FrameBuffer>
  {
    return_type code()
    {
      return arg<1>();
    }
  };

  template <>
  struct node_function_info_traits<BlendOpDst>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpDst", {"src", "dst"}, {"out"});
    }
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpDst",
        {"src", "dst"},
        "out",
        make_object<InstanceGetterFunction<BlendOpDst>>(),
        "BlendOpDst");
    }
  };
} // namespace yave