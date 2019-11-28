//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/style.hpp>

namespace yave::editor::imgui {

  // ------------------------------------------
  // Fonts

  float get_font_size(font_size_level level)
  {
    switch (level) {
      case font_size_level::display:
        return 36;
      case font_size_level::title:
        return 17;
      case font_size_level::body:
        return 13;
      case font_size_level::caption:
        return 11;
    }
    unreachable();
  }

  float get_font_weight(font_size_level level)
  {
    switch (level) {
      case font_size_level::display:
        return 200;
      case font_size_level::title:
        return 300;
      case font_size_level::body:
        return 400;
      case font_size_level::caption:
        return 400;
    }
    unreachable();
  }

  ImFont* get_font_data(font_size_level level)
  {
    (void)level;
    return ImGui::GetFont();
  }

  float get_text_padding()
  {
    return 8;
  }

  auto calc_text_size(const std::string& text, font_size_level level) -> ImVec2
  {
    auto font = get_font_data(level);
    return font->CalcTextSizeA(
      get_font_size(level),
      std::numeric_limits<float>::max(),
      0.0,
      text.c_str());
  }

  auto calc_text_pos(
    const std::string& text,
    font_size_level level,
    ImVec2 region,
    text_alignment alignment) -> ImVec2
  {
    auto text_size = calc_text_size(text, level);

    assert(region.x >= text_size.x);
    assert(region.y >= text_size.y);

    if (alignment == text_alignment::center) {
      return {(region.x - text_size.x) / 2, (region.y - text_size.y) / 2};
    }

    if (alignment == text_alignment::left) {
      return {std::min(get_text_padding(), (region.x - text_size.x) / 2),
              (region.y - text_size.y) / 2};
    }

    if (alignment == text_alignment::right) {
      return {(region.x - text_size.x) / 2,
              std::min(get_text_padding(), (region.y - text_size.y) / 2)};
    }

    unreachable();
  }

  // ------------------------------------------
  // Drawing layers

  size_t get_channel_count()
  {
    // Ch:
    // 4. Foreground layer
    // 3. Socket layer
    // 2. Node layer
    // 1. Connection layer
    // 0. Background layer
    return 5;
  }

  size_t get_background_channel_index()
  {
    return 0;
  }

  size_t get_foreground_channel_index()
  {
    return 4;
  }

  size_t get_node_channel_index()
  {
    return 2;
  }

  size_t get_socket_channel_index()
  {
    return 3;
  }
  size_t get_connection_channel_index()
  {
    return 1;
  }

  // ------------------------------------------
  // Rouding

  [[nodiscard]] float get_node_rounding()
  {
    return 3;
  }
} // namespace yave::editor::imgui