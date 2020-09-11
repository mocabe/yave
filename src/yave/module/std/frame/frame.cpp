//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/frame/frame.hpp>
#include <yave/module/std/color/color.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>

namespace yave {

  auto node_declaration_traits<node::Frame>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Frame",
      "/std/frame",
      "Create new frame buffer",
      {"color"},
      {"frame"},
      {{0, make_data_type_holder<Color>()}});
  }

  namespace modules::_std::frame {

    struct FrameBufferColored
      : NodeFunction<FrameBufferColored, Color, FrameBuffer>
    {
      frame_buffer_manager& m_fbm;
      vulkan::rgba32f_offscreen_render_pass& m_render_pass;

      FrameBufferColored(
        frame_buffer_manager& fbm,
        vulkan::rgba32f_offscreen_compositor& comp)
        : m_fbm {fbm}
        , m_render_pass {comp.render_pass()}
      {
      }

      return_type code() const
      {
        auto col = eval_arg<0>();
        auto fb  = make_object<FrameBuffer>(m_fbm.get_pool_object());

        m_render_pass.clear_texture(
          m_fbm.get_texture_data(fb->id()),
          std::array {
            static_cast<float>(col->r),
            static_cast<float>(col->g),
            static_cast<float>(col->b),
            static_cast<float>(col->a)});

        return fb;
      }
    };

  } // namespace modules::_std::frame

  auto node_definition_traits<node::Frame, modules::_std::tag>::
    get_node_definitions(
      frame_buffer_manager& mngr,
      vulkan::rgba32f_offscreen_compositor& comp)
      -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Frame>();
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::frame::FrameBufferColored>(mngr, comp),
      info.description())};
  }
} // namespace yave