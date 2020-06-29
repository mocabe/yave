//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave-imgui/style.hpp>
#include <yave/lib/imgui/extension.hpp>
#include <yave/node/core/structured_node_graph.hpp>

namespace yave::editor::imgui {

  // ------------------------------------------
  // Text layout helpers

  /// calculate text size
  [[nodiscard]] ImVec2 calc_text_size(
    const std::string& text,
    const font_size_level& level);

  /// calculate aligned text position
  [[nodiscard]] ImVec2 calc_text_pos(
    const std::string& text,
    const font_size_level& level,
    const ImVec2& region,
    const text_alignment& alignment);

} // namespace yave::editor::imgui