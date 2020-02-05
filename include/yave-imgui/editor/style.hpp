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

  [[nodiscard]] inline float gridpx(size_t n = 1)
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

  // ------------------------------------------
  // Colors

  [[nodiscard]] inline ImColor color_glow(const ImColor& col, float glow)
  {
    return {std::min(1.f, col.Value.x * (1.f + glow)),
            std::min(1.f, col.Value.y * (1.f + glow)),
            std::min(1.f, col.Value.z * (1.f + glow)),
            col.Value.w};
  }

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
    return {120, 80, 110, 230};
  }

  [[nodiscard]] inline ImColor get_node_header_fill_color_hovered()
  {
    return {120, 80, 110, 230};
  }

  [[nodiscard]] inline ImColor get_node_header_fill_color_selected()
  {
    return {120, 80, 110, 230};
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
    return {136, 133, 138, 230};
  }

  [[nodiscard]] inline ImColor get_node_edge_color_hovered()
  {
    return {255, 150, 50, 230};
  }

  [[nodiscard]] inline ImColor get_node_edge_color_selected()
  {
    return {255, 150, 50, 230};
  }

  [[nodiscard]] inline ImColor get_node_header_text_color()
  {
    return {250, 250, 250, 255};
  }

  [[nodiscard]] inline ImColor get_socket_text_color()
  {
    return {250, 250, 250, 255};
  }

  [[nodiscard]] inline ImColor get_socket_slider_color()
  {
    return {170, 181, 196, 255};
  }

  [[nodiscard]] inline ImColor get_socket_slider_text_color()
  {
    return {50, 50, 50, 255};
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
    return color_glow(get_socket_slot_color(type), 0.2);
  }

  [[nodiscard]] inline ImColor get_socket_slot_color_selected(socket_type type)
  {
    return color_glow(get_socket_slot_color(type), 0.2);
  }

  [[nodiscard]] inline ImColor get_connection_color()
  {
    return {240, 240, 240, 240};
  }

  [[nodiscard]] inline ImColor get_connection_color_hovered()
  {
    return get_connection_color();
  }

  [[nodiscard]] inline ImColor get_connection_color_selected()
  {
    return get_connection_color();
  }

  // ------------------------------------------
  // Other

  [[nodiscard]] inline float get_background_grid_size()
  {
    return gridpx(6);
  }

  [[nodiscard]] inline float get_node_rounding()
  {
    return 2;
  }

  [[nodiscard]] inline ImVec2 get_socket_padding()
  {
    return {gridpx(3) / 2, gridpx(1)};
  }

  [[nodiscard]] inline float get_socket_slot_radius()
  {
    return 5;
  }

  [[nodiscard]] inline float get_connection_thickness()
  {
    return 1.f;
  }

} // namespace yave::editor::imgui