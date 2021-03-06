//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/frame/frame.hpp>
#include <yave/signal/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/obj/color/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Frame::Frame>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Frame.Frame",
      "Create new frame buffer",
      node_declaration_visibility::_public,
      {"color"},
      {"frame"},
      {{0, make_node_argument<Color>(data::color(0.f, 0.f, 0.f, 1.f))}});
  }

  namespace modules::_std::frame {

    struct FrameBufferColored
      : SignalFunction<FrameBufferColored, Color, FrameBuffer>
    {
      data::frame_buffer_manager& m_fbm;
      vulkan::rgba32f_offscreen_render_pass& m_render_pass;

      FrameBufferColored(
        data::frame_buffer_manager& fbm,
        vulkan::rgba32f_offscreen_compositor& comp)
        : m_fbm {fbm}
        , m_render_pass {comp.render_pass()}
      {
      }

      return_type code() const
      {
        auto col = eval_arg<0>();
        auto fb  = make_object<FrameBuffer>(m_fbm.get_pool_object());

        auto c = static_cast<glm::fvec4>(*col);

        // fill
        if (c != glm::fvec4(0.f)) {
          m_render_pass.clear_texture(
            m_fbm.get_texture_data(fb->id()), std::array {c.r, c.g, c.b, c.a});
        }

        return fb;
      }
    };

  } // namespace modules::_std::frame

  auto node_definition_traits<node::Frame::Frame, modules::_std::tag>::
    get_node_definitions(
      data::frame_buffer_manager& mngr,
      vulkan::rgba32f_offscreen_compositor& comp)
      -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Frame::Frame>();
    return std::vector {node_definition(
      info.full_name(),
      0,
      make_object<modules::_std::frame::FrameBufferColored>(mngr, comp))};
  }
} // namespace yave