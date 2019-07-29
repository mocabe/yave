//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/blend_op.hpp>

#include <yave/node/obj/function.hpp>
#include <yave/node/obj/compositor.hpp>
#include <yave/data/lib/image_blend.hpp>
#include <yave/backend/default/system/config.hpp>

namespace yave {

  namespace backend::default_render {

    struct BlendOpSrc
      : NodeFunction<BlendOpSrc, FrameBuffer, FrameBuffer, FrameBuffer>
    {
      return_type code()
      {
        return eval_arg<0>();
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

  } // namespace backend::default_render

  template <>
  struct bind_info_traits<BlendOpSrc, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpSrc",
        {"src", "dst"},
        "out",
        make_object<
          InstanceGetterFunction<backend::default_render::BlendOpSrc>>(),
        "BlendOpSrc");
    }
  };

  template <>
  struct bind_info_traits<BlendOpDst, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpDst",
        {"src", "dst"},
        "out",
        make_object<
          InstanceGetterFunction<backend::default_render::BlendOpDst>>(),
        "BlendOpDst");
    }
  };

  template <>
  struct bind_info_traits<BlendOpOver, backend::tags::default_render>
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
        make_object<
          InstanceGetterFunction<backend::default_render::BlendOpOver>>(),
        "BlendOpOver");
    }
  };

  template <>
  struct bind_info_traits<BlendOpIn, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpIn",
        {"src", "dst"},
        "out",
        make_object<
          InstanceGetterFunction<backend::default_render::BlendOpIn>>(),
        "BlendOpIn");
    }
  };

  template <>
  struct bind_info_traits<BlendOpOut, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpOut",
        {"src", "dst"},
        "out",
        make_object<
          InstanceGetterFunction<backend::default_render::BlendOpOut>>(),
        "BlendOpOut");
    }
  };

  template <>
  struct bind_info_traits<BlendOpAdd, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      return bind_info(
        "BlendOpAdd",
        {"src", "dst"},
        "out",
        make_object<
          InstanceGetterFunction<backend::default_render::BlendOpAdd>>(),
        "BlendOpAdd");
    }
  };
} // namespace yave