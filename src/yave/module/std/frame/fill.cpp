//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/frame/fill.hpp>
#include <yave/module/std/frame/frame.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/obj/color/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Frame::Fill>::get_node_declaration(
    data::frame_buffer_manager& fbm) -> node_declaration
  {
    return function_node_declaration(
      "Frame.Fill",
      "Fill frame buffer with specified color",
      node_declaration_visibility::_public,
      {"color", "frame"},
      {"frame"},
      {{0, make_node_argument<Color>(data::color(0.f, 0.f, 0.f, 1.f))}});
  }

  namespace modules::_std::frame {

    struct FillFrame : NodeFunction<FillFrame, Color, FrameBuffer, FrameBuffer>
    {
      data::frame_buffer_manager& m_fbm;
      vulkan::rgba32f_offscreen_render_pass& m_render_pass;

      FillFrame(
        data::frame_buffer_manager& fbm,
        vulkan::rgba32f_offscreen_compositor& compositor)
        : m_fbm {fbm}
        , m_render_pass {compositor.render_pass()}
      {
      }

      auto code() const -> return_type
      {
        auto col = eval_arg<0>();
        auto dst = eval_arg<1>().clone();

        assert(m_fbm.exists(dst->id()));

        auto c = static_cast<glm::fvec4>(*col);

        m_render_pass.clear_texture(
          m_fbm.get_texture_data(dst->id()), std::array {c.r, c.g, c.b, c.a});

        return dst;
      }
    };
  } // namespace modules::_std::frame

  auto node_definition_traits<node::Frame::Fill, modules::_std::tag>::
    get_node_definitions(
      data::frame_buffer_manager& fbm,
      vulkan::rgba32f_offscreen_compositor& compositor)
      -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Frame::Fill>(fbm);
    return std::vector {node_definition(
      info.full_name(),
      0,
      make_object<modules::_std::frame::FillFrame>(fbm, compositor))};
  }
} // namespace yave