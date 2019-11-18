//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/class/compositor.hpp>
#include <yave/node/class/blend_op.hpp>
#include <yave/lib/image/image_blend.hpp>

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
        auto dst = clone(fb_dst);

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
        auto dst = clone(fb_dst);

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
        auto dst = clone(fb_dst);

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
        auto dst = clone(fb_dst);

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

#define YAVE_DEFAULT_RENDER_DEF_BLEND_OP(TYPE)                                 \
  template <>                                                                  \
  struct node_definition_traits<node::TYPE, backend::tags::default_render>     \
  {                                                                            \
    static auto get_node_definition() -> node_definition                       \
    {                                                                          \
      auto info = get_node_declaration<node::TYPE>();                          \
      return node_definition(                                                  \
        info.name(),                                                           \
        info.output_sockets()[0],                                              \
        info.name(),                                                           \
        make_object<InstanceGetterFunction<backend::default_render::TYPE>>()); \
    }                                                                          \
  }

  YAVE_DEFAULT_RENDER_DEF_BLEND_OP(BlendOpSrc);
  YAVE_DEFAULT_RENDER_DEF_BLEND_OP(BlendOpDst);
  YAVE_DEFAULT_RENDER_DEF_BLEND_OP(BlendOpOver);
  YAVE_DEFAULT_RENDER_DEF_BLEND_OP(BlendOpIn);
  YAVE_DEFAULT_RENDER_DEF_BLEND_OP(BlendOpOut);
  YAVE_DEFAULT_RENDER_DEF_BLEND_OP(BlendOpAdd);

} // namespace yave