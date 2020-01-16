//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave-imgui/editor/style.hpp>

#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/app/editor_context.hpp>

namespace yave::editor::imgui {

  /// Draw node graph in child window.
  void draw_node_canvas(
    yave::imgui::imgui_context& imgui_ctx,
    yave::app::editor_context& editor_ctx);

} // namespace yave::editor::imgui