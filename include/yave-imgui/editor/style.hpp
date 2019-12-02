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

  [[nodiscard]] constexpr size_t gridpx(size_t n = 1)
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
  [[nodiscard]] ImFont* get_font_data(font_size_level level);

  /// Get font size in px
  [[nodiscard]] float get_font_size(font_size_level level);

  /// Get font weight
  [[nodiscard]] float get_font_weight(font_size_level level);

  /// Text padding px
  [[nodiscard]] float get_text_padding();

  /// calculate text size
  [[nodiscard]] auto calc_text_size(
    const std::string& text,
    font_size_level size) -> ImVec2;

  /// calculate aligned text position
  [[nodiscard]] auto calc_text_pos(
    const std::string& text,
    font_size_level level,
    ImVec2 region,
    text_alignment alignment) -> ImVec2;

  // ------------------------------------------
  // Colors

  [[nodiscard]] ImColor get_node_header_fill_color();
  [[nodiscard]] ImColor get_node_header_fill_color_hovered();
  [[nodiscard]] ImColor get_node_header_fill_color_selected();

  [[nodiscard]] ImColor get_node_background_fill_color();
  [[nodiscard]] ImColor get_node_background_fill_color_hovered();
  [[nodiscard]] ImColor get_node_background_fill_color_selected();

  [[nodiscard]] ImColor get_node_edge_color();
  [[nodiscard]] ImColor get_node_edge_color_hovered();
  [[nodiscard]] ImColor get_node_edge_color_selected();

  [[nodiscard]] ImColor get_node_header_text_color();
  [[nodiscard]] ImColor get_socket_text_color();

  [[nodiscard]] ImColor get_socket_slot_color(socket_type tpye);
  [[nodiscard]] ImColor get_socket_slot_color_hovered(socket_type type);
  [[nodiscard]] ImColor get_socket_slot_color_selected(socket_type type);

  // ------------------------------------------
  // Drawing layers

  [[nodiscard]] size_t get_channel_count();
  [[nodiscard]] size_t get_background_channel_index();
  [[nodiscard]] size_t get_foreground_channel_index();
  [[nodiscard]] size_t get_node_channel_index();
  [[nodiscard]] size_t get_socket_channel_index();
  [[nodiscard]] size_t get_connection_channel_index();

  // ------------------------------------------
  // Rouding

  [[nodiscard]] float get_node_rounding();

} // namespace yave::editor::imgui