//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/imgui/extension.hpp>
#include <yave/node/core/socket_property.hpp>

namespace yave::editor::imgui {

  // ------------------------------------------
  // Grid

  [[nodiscard]] inline size_t gridpx(size_t n = 1)
  {
    return n * 8;
  }

  // ------------------------------------------
  // Fonts

  /// Font size level
  enum class font_size_level
  {
    e36,
    e22,
    e17,
    e15,
    e13,
    e11,
  };

  /// Text alignment types
  enum class text_alignment
  {
    left,
    right,
    center,
  };

  /// Get font
  [[nodiscard]] inline ImFont* get_font_data(font_size_level level)
  {
    (void)level;
    return ImGui::GetFont();
  }

  /// Get font size in px
  [[nodiscard]] inline float get_font_size(font_size_level level)
  {
    switch (level) {
      case font_size_level::e36:
        return 36;
      case font_size_level::e22:
        return 22;
      case font_size_level::e17:
        return 17;
      case font_size_level::e15:
        return 15;
      case font_size_level::e13:
        return 13;
      case font_size_level::e11:
        return 11;
    }
    unreachable();
  }

  /// Get font weight
  [[nodiscard]] inline float get_font_weight(font_size_level level)
  {
    switch (level) {
      case font_size_level::e36:
        return 300;
      case font_size_level::e22:
        return 300;
      case font_size_level::e17:
        return 300;
      case font_size_level::e15:
        return 400;
      case font_size_level::e13:
        return 400;
      case font_size_level::e11:
        return 400;
    }
    unreachable();
  }

  /// Text padding px
  [[nodiscard]] inline float get_text_padding()
  {
    return gridpx(1);
  }

  /// calculate text size
  [[nodiscard]] inline auto calc_text_size(
    const std::string& text,
    font_size_level level) -> ImVec2
  {
    auto font = get_font_data(level);
    return font->CalcTextSizeA(
      get_font_size(level),
      std::numeric_limits<float>::max(),
      0.0,
      text.c_str());
  }

  /// calculate aligned text position
  [[nodiscard]] inline auto calc_text_pos(
    const std::string& text,
    font_size_level level,
    ImVec2 region,
    text_alignment alignment) -> ImVec2
  {
    auto text_size = calc_text_size(text, level);

    assert(region.x >= text_size.x);
    assert(region.y >= text_size.y);

    switch (alignment) {
      case text_alignment::center:
        return {(region.x - text_size.x) / 2, (region.y - text_size.y) / 2};
      case text_alignment::left:
        return {std::min(get_text_padding(), (region.x - text_size.x) / 2),
                (region.y - text_size.y) / 2};
      case text_alignment::right:
        return {(region.x - text_size.x) / 2,
                std::min(get_text_padding(), (region.y - text_size.y) / 2)};
    }

    unreachable();
  }

  // ------------------------------------------
  // Colors

  [[nodiscard]] inline ImColor get_background_color()
  {
    return {53, 41, 59, 255};
  }

  [[nodiscard]] inline ImColor get_background_grid_color()
  {
    return {51, 51, 51, 255};
  }

  [[nodiscard]] inline ImColor get_node_header_fill_color()
  {
    return {120, 80, 110, 255};
  }

  [[nodiscard]] inline ImColor get_node_header_fill_color_hovered()
  {
    return {120, 80, 110, 255};
  }

  [[nodiscard]] inline ImColor get_node_header_fill_color_selected()
  {
    return {120, 80, 110, 255};
  }

  [[nodiscard]] inline ImColor get_node_background_fill_color()
  {
    return {125, 125, 125, 230};
  }

  [[nodiscard]] inline ImColor get_node_background_fill_color_hovered()
  {
    return {125, 125, 125, 230};
  }

  [[nodiscard]] inline ImColor get_node_background_fill_color_selected()
  {
    return {125, 125, 125, 230};
  }

  [[nodiscard]] inline ImColor get_node_edge_color()
  {
    return {150, 115, 165, 255};
  }

  [[nodiscard]] inline ImColor get_node_edge_color_hovered()
  {
    return {150, 115, 165, 255};
  }

  [[nodiscard]] inline ImColor get_node_edge_color_selected()
  {
    return {150, 115, 165, 255};
  }

  [[nodiscard]] inline ImColor get_node_header_text_color()
  {
    return {3, 3, 3, 255};
  }

  [[nodiscard]] inline ImColor get_socket_text_color()
  {
    return {3, 3, 3, 255};
  }

  [[nodiscard]] inline ImColor get_socket_slot_color(socket_type type)
  {
    switch (type) {
      case socket_type::input:
        return {170, 181, 196, 255};
      case socket_type::output:
        return {255, 143, 130, 255};
    }
    unreachable();
  }

  [[nodiscard]] inline ImColor get_socket_slot_color_hovered(socket_type type)
  {
    return get_socket_slot_color(type);
  }

  [[nodiscard]] inline ImColor get_socket_slot_color_selected(socket_type type)
  {
    return get_socket_slot_color(type);
  }

  // ------------------------------------------
  // Drawing layers

  [[nodiscard]] inline size_t get_channel_count()
  {
    // Ch:
    // 4. Foreground layer
    // 3. Socket layer
    // 2. Node layer
    // 1. Connection layer
    // 0. Background layer
    return 5;
  }

  [[nodiscard]] inline size_t get_background_channel_index()
  {
    return 0;
  }

  [[nodiscard]] inline size_t get_foreground_channel_index()
  {
    return 4;
  }

  [[nodiscard]] inline size_t get_node_channel_index()
  {
    return 2;
  }

  [[nodiscard]] inline size_t get_socket_channel_index()
  {
    return 3;
  }

  [[nodiscard]] inline size_t get_connection_channel_index()
  {
    return 1;
  }

  // ------------------------------------------
  // Grid

  [[nodiscard]] inline float get_background_grid_size()
  {
    return 50;
  }

  // ------------------------------------------
  // Rouding

  [[nodiscard]] inline float get_node_rounding()
  {
    return 2;
  }

} // namespace yave::editor::imgui