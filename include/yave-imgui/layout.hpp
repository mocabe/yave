//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave-imgui/style.hpp>
#include <yave/lib/imgui/extension.hpp>
#include <yave/node/core/structured_node_graph.hpp>
#include <yave/app/editor_context.hpp>

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

  // ------------------------------------------
  // Socket layout

  [[nodiscard]] ImVec2 calc_socket_min_area_size(
    const socket_handle& socket,
    const structured_node_graph& g);

  [[nodiscard]] ImVec2 calc_socket_size(
    const ImVec2& node_size,
    const socket_handle& socket,
    const structured_node_graph& g);

  [[nodiscard]] ImVec2 calc_socket_slot_relpos(
    const ImVec2& node_size,
    const socket_handle& s,
    const structured_node_graph& g);

  [[nodiscard]] ImVec2 calc_socket_slot_pos(
    const socket_type& type,
    const ImVec2& base,
    const ImVec2& size);

  // ------------------------------------------
  // Node layout

  /// minimum size of node header
  [[nodiscard]] ImVec2 calc_node_header_min_size(const std::string& title);

  /// calculate node size
  [[nodiscard]] ImVec2 calc_node_background_size(
    const node_handle& node,
    const structured_node_graph& g);

  /// calculate node header size
  [[nodiscard]] ImVec2 calc_node_header_size(
    const ImVec2& node_size,
    const node_handle& node,
    const structured_node_graph& g);

  /// Runtime socket layout data
  struct socket_layout
  {
    /// screen pos
    ImVec2 pos;
    /// size
    ImVec2 size;
    /// inner area pos
    ImVec2 area_pos;
    /// inner area size
    ImVec2 area_size;
    /// slot screen pos
    ImVec2 slot_pos;
  };

  /// Runtime node layout data
  struct node_layout
  {
    /// screen pos
    ImVec2 pos;
    /// size
    ImVec2 size;
    /// header size
    ImVec2 header_size;
    /// socket area screen pos
    ImVec2 socket_area_pos;
    /// socket area size
    ImVec2 socket_area_size;
    /// socket layouts
    std::map<socket_handle, socket_layout> sockets;
    /// channel index.
    size_t channel_index;
  };

  /// Canvas draw layout
  struct canvas_layout
  {
    // nodes used to create layout
    std::vector<node_handle> nodes;
    // layout tree
    std::map<node_handle, node_layout> map;
    // channel size including subchannels
    size_t channel_size;
    // background channel index
    size_t background_channel_index;
    // connection channel index
    size_t connection_channel_index;
    // node channel index (which is base index of node subchannels)
    size_t node_channel_index;
    // foreground index
    size_t foreground_channel_index;
  };

  /// Claculate node layout
  [[nodiscard]] auto calc_node_layout(
    const ImVec2& node_screen_pos,
    const node_handle& node,
    const structured_node_graph& g) -> node_layout;

  /// Calculate socket layout
  [[nodiscard]] auto calc_socket_layout(
    const ImVec2& socket_screen_pos,
    const ImVec2& node_size,
    const socket_handle& socket,
    const structured_node_graph& g) -> socket_layout;

  /// Calculate layout of canvas from given list of nodes.
  /// All layout information will be stored in map tree structure.
  [[nodiscard]] auto calc_canvas_layout(
    const std::vector<node_handle>& nodes,
    const app::editor_context& editor_ctx) -> canvas_layout;

} // namespace yave::editor::imgui