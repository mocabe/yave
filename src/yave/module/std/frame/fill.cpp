//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/frame/fill.hpp>
#include <yave/module/std/frame/frame.hpp>
#include <yave/module/std/color/color.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>

namespace yave {

  auto node_declaration_traits<node::FillFrame>::get_node_declaration(
    data::frame_buffer_manager& fbm) -> node_declaration
  {
    return node_declaration(
      "Fill",
      "/std/frame",
      "Fill frame buffer with specified color",
      {"color", "frame"},
      {"frame"},
      {{0, make_data_type_holder<Color>()},
       {1,
        make_object<modules::_std::frame::FrameBufferConstructor>(
          fbm.get_pool_object())}});
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

        if (!m_fbm.exists(dst->id()))
          assert(!"TODO");

        m_render_pass.clear_texture(
          m_fbm.get_texture_data(dst->id()),
          std::array {
            static_cast<float>(col->r),
            static_cast<float>(col->g),
            static_cast<float>(col->b),
            static_cast<float>(col->a)});

        return dst;
      }
    };
  } // namespace modules::_std::frame

  auto node_definition_traits<node::FillFrame, modules::_std::tag>::
    get_node_definitions(
      data::frame_buffer_manager& fbm,
      vulkan::rgba32f_offscreen_compositor& compositor)
      -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::FillFrame>(fbm);
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::frame::FillFrame>(fbm, compositor),
      info.description())};
  }
} // namespace yave