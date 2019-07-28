//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/function.hpp>
#include <yave/node/obj/compositor.hpp>
#include <yave/data/lib/blend_operation.hpp>
#include <yave/data/lib/image_blend.hpp>

namespace yave {

  struct BlendOpSrc;
  struct BlendOpDst;
  struct BlendOpOver;
  struct BlendOpIn;
  struct BlendOpOut;
  struct BlendOpAdd;

  /// Get node info from blend_operation
  node_info get_blend_op_node_info(blend_operation op)
  {
    switch (op) {
      case blend_operation::src:
        return get_node_info<BlendOpSrc>();
      case blend_operation::dst:
        return get_node_info<BlendOpDst>();
      case blend_operation::over:
        return get_node_info<BlendOpOver>();
      case blend_operation::in:
        return get_node_info<BlendOpIn>();
      case blend_operation::out:
        return get_node_info<BlendOpOut>();
      case blend_operation::add:
        return get_node_info<BlendOpAdd>();
    }
    // default: alpha overlay
    return get_node_info<BlendOpOver>();
  }

  /// Get bind info from blend_operation
  bind_info get_blend_op_bind_info(blend_operation op)
  {
    switch (op) {
      case blend_operation::src:
        return get_bind_info<BlendOpSrc>();
      case blend_operation::dst:
        return get_bind_info<BlendOpDst>();
      case blend_operation::over:
        return get_bind_info<BlendOpOver>();
      case blend_operation::in:
        return get_bind_info<BlendOpIn>();
      case blend_operation::out:
        return get_bind_info<BlendOpOut>();
      case blend_operation::add:
        return get_bind_info<BlendOpAdd>();
    }
    // default: alpha overlay
    return get_bind_info<BlendOpOver>();
  }

  /// Get list of node info from blend_operation
  std::vector<node_info> get_blend_op_node_info_list()
  {
    std::vector<node_info> ret = {
      get_blend_op_node_info(blend_operation::src),
      get_blend_op_node_info(blend_operation::dst),
      get_blend_op_node_info(blend_operation::over),
      get_blend_op_node_info(blend_operation::in),
      get_blend_op_node_info(blend_operation::out),
      get_blend_op_node_info(blend_operation::add),
    };
    return ret;
  }

  /// Get list of bind info from blend_operation
  std::vector<bind_info> get_blend_op_bind_info_list()
  {
    std::vector<bind_info> ret = {
      get_blend_op_bind_info(blend_operation::src),
      get_blend_op_bind_info(blend_operation::dst),
      get_blend_op_bind_info(blend_operation::over),
      get_blend_op_bind_info(blend_operation::in),
      get_blend_op_bind_info(blend_operation::out),
      get_blend_op_bind_info(blend_operation::add),
    };
    return ret;
  }

  struct BlendOpSrc
    : NodeFunction<BlendOpSrc, FrameBuffer, FrameBuffer, FrameBuffer>
  {
    return_type code()
    {
      return eval_arg<0>();
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
      return eval_arg<1>();
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

  struct BlendOpOver
    : NodeFunction<BlendOpOver, FrameBuffer, FrameBuffer, FrameBuffer>
  {
    return_type code() const
    {
      auto fb_src = eval_arg<0>();
      auto fb_dst = eval_arg<1>();

      // clone dst
      auto dst = fb_dst->copy();

      const_image_view src_view   = fb_src->get_image_view();
      mutable_image_view dst_view = dst->get_image_view();

      if (src_view.image_format() == image_format::RGBA8UI) {
        alpha_blend_RGBA8UI(src_view, dst_view, blend_operation::over);
        return dst;
      }

      if (src_view.image_format() == image_format::RGBA16UI) {
        alpha_blend_RGBA16UI(src_view, dst_view, blend_operation::over);
        return dst;
      }

      if (src_view.image_format() == image_format::RGBA32F) {
        alpha_blend_RGBA32F(src_view, dst_view, blend_operation::over);
        return dst;
      }

      throw std::runtime_error("Invalid frame buffer type");
    }
  };

  template <>
  struct node_function_info_traits<BlendOpOver>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpOver", {"src", "dst"}, {"out"});
    }
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpOver",
        {"src", "dst"},
        "out",
        make_object<InstanceGetterFunction<BlendOpOver>>(),
        "BlendOpOver");
    }
  };

  struct BlendOpIn
    : NodeFunction<BlendOpIn, FrameBuffer, FrameBuffer, FrameBuffer>
  {
    return_type code() const
    {
      auto fb_src = eval_arg<0>();
      auto fb_dst = eval_arg<1>();

      // clone dst
      auto dst = fb_dst->copy();

      const_image_view src_view   = fb_src->get_image_view();
      mutable_image_view dst_view = dst->get_image_view();

      if (src_view.image_format() == image_format::RGBA8UI) {
        alpha_blend_RGBA8UI(src_view, dst_view, blend_operation::in);
        return dst;
      }

      if (src_view.image_format() == image_format::RGBA16UI) {
        alpha_blend_RGBA16UI(src_view, dst_view, blend_operation::in);
        return dst;
      }

      if (src_view.image_format() == image_format::RGBA32F) {
        alpha_blend_RGBA32F(src_view, dst_view, blend_operation::in);
        return dst;
      }

      throw std::runtime_error("Invalid frame buffer type");
    }
  };

  template <>
  struct node_function_info_traits<BlendOpIn>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpIn", {"src", "dst"}, {"out"});
    }
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpIn",
        {"src", "dst"},
        "out",
        make_object<InstanceGetterFunction<BlendOpIn>>(),
        "BlendOpIn");
    }
  };

  struct BlendOpOut
    : NodeFunction<BlendOpOut, FrameBuffer, FrameBuffer, FrameBuffer>
  {
    return_type code() const
    {
      auto fb_src = eval_arg<0>();
      auto fb_dst = eval_arg<1>();

      // clone dst
      auto dst = fb_dst->copy();

      const_image_view src_view   = fb_src->get_image_view();
      mutable_image_view dst_view = dst->get_image_view();

      if (src_view.image_format() == image_format::RGBA8UI) {
        alpha_blend_RGBA8UI(src_view, dst_view, blend_operation::out);
        return dst;
      }

      if (src_view.image_format() == image_format::RGBA16UI) {
        alpha_blend_RGBA16UI(src_view, dst_view, blend_operation::out);
        return dst;
      }

      if (src_view.image_format() == image_format::RGBA32F) {
        alpha_blend_RGBA32F(src_view, dst_view, blend_operation::out);
        return dst;
      }

      throw std::runtime_error("Invalid frame buffer type");
    }
  };

  template <>
  struct node_function_info_traits<BlendOpOut>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpOut", {"src", "dst"}, {"out"});
    }
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpOut",
        {"src", "dst"},
        "out",
        make_object<InstanceGetterFunction<BlendOpOut>>(),
        "BlendOpOut");
    }
  };

  struct BlendOpAdd
    : NodeFunction<BlendOpAdd, FrameBuffer, FrameBuffer, FrameBuffer>
  {
    return_type code() const
    {
      auto fb_src = eval_arg<0>();
      auto fb_dst = eval_arg<1>();

      // clone dst
      auto dst = fb_dst->copy();

      const_image_view src_view   = fb_src->get_image_view();
      mutable_image_view dst_view = dst->get_image_view();

      if (src_view.image_format() == image_format::RGBA8UI) {
        alpha_blend_RGBA8UI(src_view, dst_view, blend_operation::add);
        return dst;
      }

      if (src_view.image_format() == image_format::RGBA16UI) {
        alpha_blend_RGBA16UI(src_view, dst_view, blend_operation::add);
        return dst;
      }

      if (src_view.image_format() == image_format::RGBA32F) {
        alpha_blend_RGBA32F(src_view, dst_view, blend_operation::add);
        return dst;
      }

      throw std::runtime_error("Invalid frame buffer type");
    }
  };

  template <>
  struct node_function_info_traits<BlendOpAdd>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpAdd", {"src", "dst"}, {"out"});
    }
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpAdd",
        {"src", "dst"},
        "out",
        make_object<InstanceGetterFunction<BlendOpAdd>>(),
        "BlendOpAdd");
    }
  };
} // namespace yave