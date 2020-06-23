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

  // ------------------------------------------
  // Socket layout

  ImVec2 calc_socket_min_area_size(
    const socket_handle& socket,
    const structured_node_graph& g)
  {
    assert(g.exists(socket));
    auto name      = g.get_name(socket);
    auto text_size = calc_text_size(*name, font_size_level::e15);
    auto padding   = get_socket_padding();
    // fix 16 px height
    return {text_size.x + 2 * padding.x, gridpx(2) + 2 * padding.y};
  }

  ImVec2 calc_socket_size(
    const ImVec2& node_size,
    const socket_handle& socket,
    const structured_node_graph& g)
  {
    auto ssmin = calc_socket_min_area_size(socket, g);
    return {node_size.x, ssmin.y};
  }

  ImVec2 calc_socket_slot_relpos(
    const ImVec2& node_size,
    const socket_handle& s,
    const structured_node_graph& g)
  {
    auto size = calc_socket_size(node_size, s, g);
    auto type = g.get_info(s)->type();

    switch (type) {
      case socket_type::input:
        return {size.x, size.y / 2};
      case socket_type::output:
        return {0, size.y / 2};
    }
    unreachable();
  }

  ImVec2 calc_socket_slot_pos(
    const socket_type& type,
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
  auto calc_node_header_min_size(const std::string& title) -> ImVec2
  {
    // Width: text width + 32 px
    // Height: 32 px
    // Font: 15 px
    auto text_size = calc_text_size(title, font_size_level::e15);
    auto min_width = 3 * gridpx(6);
    return {std::max(text_size.x + 2 * gridpx(4), min_width), gridpx(4)};
  }

  /// calculate node size
  auto calc_node_background_size(
    const node_handle& node,
    const structured_node_graph& g) -> ImVec2
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

    // set minimum node width
    size.x = std::max(size.x, get_background_grid_size() * 3);

    return size;
  }

  /// calculate node header size
  auto calc_node_header_size(
    const ImVec2& node_size,
    const node_handle& node,
    const structured_node_graph& g) -> ImVec2
  {
    auto title = *g.get_name(node);
    auto min   = calc_node_header_min_size(title);
    return {node_size.x, min.y};
  }

} // namespace yave::editor::imgui