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

namespace yave::editor {

  /// render result view window
  class render_view_window : public wm::window
  {
    /// imgui context
    yave::imgui::imgui_context& imgui_ctx;

    const char* res_tex_name = "render_view_res";
    const char* bg_tex_name  = "render_view_bg";

    uint32_t width, height;
    image_format frame_format;

    ImVec2 scroll = {0, 0};
    float scale   = 1.f;

    std::chrono::steady_clock::time_point last_exec_time;

  public:
    render_view_window(yave::imgui::imgui_context& imctx);

  public:
    void update(editor::data_context& data_ctx, editor::view_context& view_ctx)
      override;

    void draw(editor::data_context& data_ctx, editor::view_context& view_ctx)
      const override;
  };
} // namespace yave::editor