//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/obj/blend_op.hpp>
#include <yave/node/core/function.hpp>
#include <yave/node/obj/compositor.hpp>
#include <yave/lib/image/image_blend.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>

namespace yave {

  namespace backend::default_render {

    struct BlendOpSrc
      : NodeFunction<BlendOpSrc, FrameBuffer, FrameBuffer, FrameBuffer>
    {
      return_type code() const
      {
        return eval_arg<0>();
      }
    };

    struct BlendOpDst
      : NodeFunction<BlendOpDst, FrameBuffer, FrameBuffer, FrameBuffer>
    {
      return_type code() const
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

#define YAVE_DECL_BLEND_OP_BIND_INFO_DEFAULT_RENDER(TYPE)                     \
  template <>                                                                 \
  struct bind_info_traits<node::TYPE, backend::tags::default_render>          \
  {                                                                           \
    static bind_info get_bind_info()                                          \
    {                                                                         \
      auto info = get_node_info<node::TYPE>();                                \
      return bind_info(                                                       \
        info.name(),                                                          \
        info.input_sockets(),                                                 \
        info.output_sockets()[0],                                             \
        make_object<InstanceGetterFunction<backend::default_render::TYPE>>(), \
        info.name());                                                         \
    }                                                                         \
  }

  YAVE_DECL_BLEND_OP_BIND_INFO_DEFAULT_RENDER(BlendOpSrc);
  YAVE_DECL_BLEND_OP_BIND_INFO_DEFAULT_RENDER(BlendOpDst);
  YAVE_DECL_BLEND_OP_BIND_INFO_DEFAULT_RENDER(BlendOpOver);
  YAVE_DECL_BLEND_OP_BIND_INFO_DEFAULT_RENDER(BlendOpIn);
  YAVE_DECL_BLEND_OP_BIND_INFO_DEFAULT_RENDER(BlendOpOut);
  YAVE_DECL_BLEND_OP_BIND_INFO_DEFAULT_RENDER(BlendOpAdd);

} // namespace yave