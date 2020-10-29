//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/draw.hpp>
#include <yave/lib/shape/draw.hpp>
#include <yave/lib/image/image_blend.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/obj/color/property.hpp>
#include <yave/obj/primitive/property.hpp>
#include <yave/obj/vec/vec.hpp>

namespace yave {

  auto node_declaration_traits<node::Shape::Fill>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Shape.Fill",
      "Fill shape",
      node_declaration_visibility::_public,
      {"shape", "color"},
      {"shape"},
      {{1, make_node_argument<Color>(glm::fvec4(0.f))}});
  }

  auto node_declaration_traits<node::Shape::Stroke>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Shape.Stroke",
      "Stroke shape",
      node_declaration_visibility::_public,
      {"shape", "color", "width"},
      {"shape"},
      {{1, make_node_argument<Color>(glm::fvec4(0.f))},
       {2,
        make_node_argument<Float>(
          0.f, 0.f, std::numeric_limits<float>::max(), 0.1f)}});
  }

  auto node_declaration_traits<node::Shape::Draw>::get_node_declaration(
    data::frame_buffer_manager& fbm) -> node_declaration
  {
    return function_node_declaration(
      "Shape.Draw",
      "Render shape to frame buffer",
      node_declaration_visibility::_public,
      {"shape"},
      {"frame"});
  }

  namespace modules::_std::shape {

    struct FillShape : NodeFunction<FillShape, Shape, Color, Shape>
    {
      auto code() const -> return_type
      {
        auto shape = eval_arg<0>();
        auto col   = eval_arg<1>();

        auto s = yave::shape(*shape);

        s.fill(glm::fvec4(*col));

        return make_object<Shape>(std::move(s));
      }
    };

    struct StrokeShape : NodeFunction<StrokeShape, Shape, Color, Float, Shape>
    {
      auto code() const -> return_type
      {
        auto shape = eval_arg<0>();
        auto col   = eval_arg<1>();
        auto width = eval_arg<2>();

        auto s = yave::shape(*shape);

        s.stroke(glm::fvec4(*col), *width);

        return make_object<Shape>(std::move(s));
      }
    };

    struct DrawShape : NodeFunction<DrawShape, Shape, FrameBuffer>
    {
      data::frame_buffer_manager& m_fbm;
      vulkan::rgba32f_offscreen_compositor& m_compositor;

      DrawShape(
        data::frame_buffer_manager& fbm,
        vulkan::rgba32f_offscreen_compositor& compos)
        : m_fbm {fbm}
        , m_compositor {compos}
      {
      }

      auto code() const -> return_type
      {
        auto shape = eval_arg<0>();
        auto fb    = make_object<FrameBuffer>(m_fbm.get_pool_object());

        if (!m_fbm.exists(fb->id()))
          assert(!"TODO");

        auto img =
          draw_shape_bgra8(yave::shape(*shape), fb->width(), fb->height());

        auto tex = m_compositor.render_pass().create_texture(
          {fb->width(), fb->height()}, vk::Format::eB8G8R8A8Unorm);

        // upload image to GPU
        m_compositor.render_pass().write_texture(
          tex, {}, {fb->width(), fb->height()}, img.data());

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

  auto node_definition_traits<node::Shape::Fill, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Shape::Fill>();
    return {node_definition(
      get_full_name(info), 0, make_object<modules::_std::shape::FillShape>())};
  }

  auto node_definition_traits<node::Shape::Stroke, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Shape::Stroke>();
    return {node_definition(
      get_full_name(info),
      0,
      make_object<modules::_std::shape::StrokeShape>())};
  }

  auto node_definition_traits<node::Shape::Draw, modules::_std::tag>::
    get_node_definitions(
      data::frame_buffer_manager& fbm,
      vulkan::rgba32f_offscreen_compositor& compositor)
      -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Shape::Draw>(fbm);
    return std::vector {node_definition(
      get_full_name(info),
      0,
      make_object<modules::_std::shape::DrawShape>(fbm, compositor))};
  }

} // namespace yave