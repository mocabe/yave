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

} // namespace yave::editor::imgui