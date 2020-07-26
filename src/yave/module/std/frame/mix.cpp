//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/frame/mix.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>
#include <yave/lib/image/image_blend.hpp>
#include <yave/lib/vulkan/offscreen_compositor.hpp>

namespace yave {

  auto node_declaration_traits<node::MixFrame>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Mix", "/std/frame", "Mix frames", {"src", "dst"}, {"frame"});
  }

  namespace modules::_std::frame {

    struct MixFrame
      : NodeFunction<MixFrame, FrameBuffer, FrameBuffer, FrameBuffer>
    {
      frame_buffer_manager& m_fbm;
      vulkan::rgba32f_offscreen_compositor& m_compositor;
      vulkan::rgba32f_offscreen_render_pass& m_render_pass;

      MixFrame(
        frame_buffer_manager& fbm,
        vulkan::rgba32f_offscreen_compositor& compositor)
        : m_fbm {fbm}
        , m_compositor {compositor}
        , m_render_pass {compositor.render_pass()}
      {
      }

      auto code() const -> return_type
      {
        auto src = eval_arg<0>();
        auto dst = eval_arg<1>().clone();

        auto w   = src->width();
        auto h   = src->height();

        if (!m_fbm.exists(src->id()) || !m_fbm.exists(dst->id()))
          assert(!"TODO");

        m_compositor.compose_source(m_fbm.get_texture_data(dst->id()));
        m_compositor.compose_over(m_fbm.get_texture_data(src->id()));
        m_compositor.render_pass().load_frame(
          m_fbm.get_texture_data(dst->id()), {}, {}, {w, h});

        return dst;
      }
    };
  } // namespace modules::_std::render

  auto node_definition_traits<node::MixFrame, modules::_std::tag>::
    get_node_definitions(
      frame_buffer_manager& fbm,
      vulkan::rgba32f_offscreen_compositor& compositor)
      -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::MixFrame>();
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::frame::MixFrame>(fbm, compositor),
      info.description())};
  }
}