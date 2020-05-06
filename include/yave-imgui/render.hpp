//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/lib/scene/scene_config.hpp>
#include <yave/app/editor_context.hpp>

namespace yave::editor::imgui {

  /// Draw render result
  void draw_render_window(
    yave::imgui::imgui_context& imgui_ctx,
    yave::app::editor_context& editor_ctx,
    const scene_config& config,
    const char* render_result_text_name,
    const char* render_background_tex_name);

} // namespace yave::editor::imgui