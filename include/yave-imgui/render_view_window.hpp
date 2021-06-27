//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
#include <yave/editor/data_context.hpp>
#include <yave/editor/view_context.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/lib/image/image_format.hpp>
#include <yave/media/time.hpp>
#include <yave/lib/image/image.hpp>

namespace yave::editor {

  /// render result view window
  class render_view_window : public wm::window
  {
    /// imgui context
    yave::imgui::imgui_context& imgui_ctx;

    yave::vulkan::texture_data bg_tex_data;
    ImTextureID bg_tex_id;

    yave::vulkan::texture_data res_tex_data;
    ImTextureID res_tex_id;

    uint32_t width, height;
    image_format frame_format;

    ImVec2 tex_scroll = {0, 0};
    float tex_scale   = 1.f;

    // for execution
    media::time arg_time                                = {};
    media::time last_arg_time                           = {};
    std::shared_ptr<const yave::image> last_result      = {};
    std::chrono::steady_clock::time_point last_exec_bgn = {};
    std::chrono::steady_clock::time_point last_exec_end = {};
    std::chrono::nanoseconds last_compute_time          = {};

    // for continuout execution
    uint32_t current_fps;
    bool continuous_execution = false;
    bool loop_execution       = false;
    media::time loop_time_min = media::time::zero();
    media::time loop_time_max = media::time::max();

  public:
    render_view_window(yave::imgui::imgui_context& imctx);
    ~render_view_window() noexcept;

  public:
    void set_continuous_execution(bool b);
    void set_loop_execution(bool b);
    void set_loop_execution_range(media::time min, media::time max);

  public:
    void update(editor::data_context& data_ctx, editor::view_context& view_ctx)
      override;

    void draw(
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
  };
} // namespace yave::editor