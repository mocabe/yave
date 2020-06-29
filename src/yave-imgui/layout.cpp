//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/layout.hpp>

namespace yave::editor::imgui {

  // ------------------------------------------
  // Text layout helpers

  /// calculate text size
  auto calc_text_size(const std::string& text, const font_size_level& level)
    -> ImVec2
  {
    auto font = get_font_data(level);
    return font->CalcTextSizeA(
      get_font_size(level),
      std::numeric_limits<float>::max(),
      0.0,
      text.c_str());
  }

  /// calculate aligned text position
  auto calc_text_pos(
    const std::string& text,
    const font_size_level& level,
    const ImVec2& region,
    const text_alignment& alignment) -> ImVec2
  {
    auto text_size = calc_text_size(text, level);

    assert(region.x >= text_size.x);
    assert(region.y >= text_size.y);

    switch (alignment) {
      case text_alignment::center:
        return {(region.x - text_size.x) / 2, (region.y - text_size.y) / 2};
      case text_alignment::left:
        return {get_text_padding(), (region.y - text_size.y) / 2};
      case text_alignment::right:
        return {region.x - (text_size.x + get_text_padding()),
                (region.y - text_size.y) / 2};
    }

    unreachable();
  }

} // namespace yave::editor::imgui