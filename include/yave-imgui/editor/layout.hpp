//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/imgui/extension.hpp>
#include <yave-imgui/editor/style.hpp>

#include <yave/node/core/managed_node_graph.hpp> 

namespace yave::editor::imgui {

  // ------------------------------------------
  // Text layout helpers

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
  // Socket layout

  [[nodiscard]] ImVec2 calc_socket_min_area_size(
    const socket_handle& socket,
    const managed_node_graph& g)
  {
    assert(g.exists(socket));
    auto name      = g.get_name(socket);
    auto text_size = calc_text_size(*name, font_size_level::e15);
    auto padding   = get_socket_padding();
    // fix 16 px height
    return {text_size.x + 2 * padding.x, gridpx(2) + 2 * padding.y};
  }

  // fwd
  auto calc_node_background_size(
    const node_handle& node,
    const managed_node_graph& g) -> ImVec2;

  [[nodiscard]] ImVec2 calc_socket_size(
    const node_handle& node,
    const socket_handle& socket,
    const managed_node_graph& g)
  {
    auto nsmin = calc_node_background_size(node, g);
    auto ssmin = calc_socket_min_area_size(socket, g);

    return {nsmin.x, ssmin.y};
  }

  [[nodiscard]] ImVec2
    calc_socket_slot_pos(
      socket_type type,
      const ImVec2& base,
      const ImVec2& size)
  {
    switch (type) {
      case socket_type::input:
        return {base.x + size.x, base.y + size.y / 2};
      case socket_type::output:
        return {base.x, base.y + size.y / 2};
    }
    unreachable();
  }

  // ------------------------------------------
  // Node layout

  /// minimum size of node header
  [[nodiscard]] inline auto calc_node_header_min_size(const std::string& title)
    -> ImVec2
  {
    // Width: text width + 32 px
    // Height: 32 px
    // Font: 15 px
    auto text_size = calc_text_size(title, font_size_level::e15);
    return {text_size.x + 2 * gridpx(4), gridpx(4)};
  }

  /// calculate node size
  [[nodiscard]] inline auto calc_node_background_size(
    const node_handle& node,
    const managed_node_graph& g) -> ImVec2
  {
    assert(g.exists(node));

    // min header size
    auto name = g.get_name(node);
    auto size = calc_node_header_min_size(*name);

    for (auto&& s : g.output_sockets(node)) {
      auto sz = calc_socket_min_area_size(s, g);
      size    = {std::max(size.x, sz.x), size.y + sz.y};
    }

    for (auto&& s : g.input_sockets(node)) {
      auto sz = calc_socket_min_area_size(s, g);
      size    = {std::max(size.x, sz.x), size.y + sz.y};
    }

    return size;
  }

  /// calculate node header size
  [[nodiscard]] inline auto calc_node_header_size(
    const std::string& title,
    const ImVec2& bg_size) -> ImVec2
  {
    auto min = calc_node_header_min_size(title);
    return {bg_size.x, min.y};
  }

} // namespace yave::editor::imgui