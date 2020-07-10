//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/draw.hpp>
#include <yave/module/std/primitive/data_holder.hpp>
#include <yave/module/std/render/frame.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/lib/shape/draw.hpp>
#include <yave/obj/color/color.hpp>
#include <yave/obj/vec/vec.hpp>
#include <yave/lib/image/image_blend.hpp>

namespace yave {

  auto node_declaration_traits<node::DrawShape>::get_node_declaration(
    frame_buffer_manager& fbm) -> node_declaration
  {
    return node_declaration(
      "DrawShape",
      "/std/shape",
      "Render shape to frame buffer",
      {"shape", "fill color", "stroke color", "stroke width", "frame"},
      {"result"},
      {{3, make_data_type_holder<Float>(10.f)},
       {4,
        make_object<modules::_std::render::FrameBufferConstructor>(
          fbm.get_pool_object())}});
  }

  namespace modules::_std::shape {

    struct DrawShape : NodeFunction<
                         DrawShape,
                         Shape,
                         Color,
                         Color,
                         Float,
                         FrameBuffer,
                         FrameBuffer>
    {
      frame_buffer_manager& m_fbm;
      vulkan::rgba32f_offscreen_compositor& m_compositor;

      DrawShape(
        frame_buffer_manager& fbm,
        vulkan::rgba32f_offscreen_compositor& compos)
        : m_fbm {fbm}
        , m_compositor {compos}
      {
      }

      auto code() const -> return_type
      {
        auto shape  = eval_arg<0>();
        auto fill   = eval_arg<1>();
        auto stroke = eval_arg<2>();
        auto width  = eval_arg<3>();
        auto fb     = eval_arg<4>().clone();

        if (!m_fbm.exists(fb->id()))
          assert(!"TODO");

        auto style         = shape_draw_style();
        style.fill_color   = *fill;
        style.stroke_color = *stroke;
        style.stroke_width = *width;

        auto img = draw_shape_bgra8(*shape, style, fb->width(), fb->height());

        auto tex = m_compositor.render_pass().create_texture(
          {fb->width(), fb->height()}, vk::Format::eB8G8R8A8Unorm);

        // upload image to GPU
        m_compositor.render_pass().write_texture(
          tex, {}, {fb->width(), fb->height()}, (const uint8_t*)img.data());

        auto& fbtex = m_fbm.get_texture_data(fb->id());

        // compose over input frame
        m_compositor.compose_source(fbtex);
        m_compositor.compose_over(tex);
        m_compositor.render_pass().load_frame(
          fbtex, {}, {}, {fb->width(), fb->height()});

        return fb;
      }
    };
  } // namespace modules::_std::shape

  auto node_definition_traits<node::DrawShape, modules::_std::tag>::
    get_node_definitions(
      frame_buffer_manager& fbm,
      vulkan::rgba32f_offscreen_compositor& compositor)
      -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::DrawShape>(fbm);
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::shape::DrawShape>(fbm, compositor),
      info.description())};
  }

} // namespace yave