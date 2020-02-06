//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/layout.hpp>

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
    const managed_node_graph& g)
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
    const managed_node_graph& g)
  {
    auto ssmin = calc_socket_min_area_size(socket, g);
    return {node_size.x, ssmin.y};
  }

  ImVec2 calc_socket_slot_relpos(
    const ImVec2& node_size,
    const socket_handle& s,
    const managed_node_graph& g)
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
    return {text_size.x + 2 * gridpx(4), gridpx(4)};
  }

  /// calculate node size
  auto calc_node_background_size(
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

    // set minimum node width
    size.x = std::max(size.x, get_background_grid_size() * 3);

    return size;
  }

  /// calculate node header size
  auto calc_node_header_size(
    const ImVec2& node_size,
    const node_handle& node,
    const managed_node_graph& g) -> ImVec2
  {
    auto title = *g.get_name(node);
    auto min   = calc_node_header_min_size(title);
    return {node_size.x, min.y};
  }

  /// Claculate node layout
  auto calc_node_layout(
    const ImVec2& node_screen_pos,
    const node_handle& node,
    const managed_node_graph& g) -> node_layout
  {
    using namespace yave::imgui;

    auto node_name = *g.get_name(node);

    // calc node layout
    auto node_size        = calc_node_background_size(node, g);
    auto node_header_size = calc_node_header_size(node_size, node, g);

    // layout
    node_layout nlayout;
    nlayout.pos         = node_screen_pos;
    nlayout.size        = node_size;
    nlayout.header_size = node_header_size;

    // socket area cursor
    ImVec2 cursor = {0, node_header_size.y};

    nlayout.socket_area_pos  = node_screen_pos + cursor;
    nlayout.socket_area_size = node_size - cursor;

    return nlayout;
  }

  auto calc_socket_layout(
    const ImVec2& socket_screen_pos,
    const ImVec2& node_size,
    const socket_handle& socket,
    const managed_node_graph& g) -> socket_layout
  {
    using namespace yave::imgui;

    auto socket_size            = calc_socket_size(node_size, socket, g);
    auto socket_slot_pos        = calc_socket_slot_relpos(node_size, socket, g);
    auto socket_slot_screen_pos = socket_screen_pos + socket_slot_pos;

    // layout
    socket_layout slayout;
    slayout.pos      = socket_screen_pos;
    slayout.size     = socket_size;
    slayout.slot_pos = socket_slot_screen_pos;

    // cauclate padding
    ImVec2 padding = {gridpx(1), gridpx(1)};

    slayout.area_pos  = slayout.pos + padding;
    slayout.area_size = slayout.size - padding * 2;

    return slayout;
  }

  /// Calculate layout of canvas from given list of nodes.
  /// All layout information will be stored in map tree structure.
  auto calc_canvas_layout(
    const std::vector<node_handle>& nodes,
    const app::editor_context& editor_ctx) -> canvas_layout
  {
    using namespace yave::imgui;

    canvas_layout layout;
    layout.nodes = nodes;

    // set channel index
    layout.background_channel_index = 0;
    layout.connection_channel_index = 1;
    layout.node_channel_index       = 2; // includes nodes.size() subchannels
    layout.foreground_channel_index = 2 + nodes.size();
    layout.channel_size             = 3 + nodes.size();

    auto wpos   = ImGui::GetWindowPos();
    auto scroll = to_ImVec2(editor_ctx.get_scroll());

    auto& g = editor_ctx.node_graph();

    for (auto&& n : nodes)
      assert(g.exists(n));

    for (size_t i = 0; i < nodes.size(); ++i) {

      auto& n = nodes[i];

      auto node_screen_pos = wpos + scroll + to_ImVec2(*editor_ctx.get_pos(n));

      // handle dragged node
      if (editor_ctx.get_state() == app::editor_state::node) {
        if (editor_ctx.is_selected(n)) {
          auto drag_src_pos = to_ImVec2(editor_ctx.get_drag_source_pos());
          node_screen_pos =
            node_screen_pos + ImGui::GetMousePos() - drag_src_pos;
        }
      }

      // layout
      node_layout nlayout = calc_node_layout(node_screen_pos, n, g);

      // set subchannel index
      nlayout.channel_index = layout.node_channel_index + i;

      // calc socket layouts

      ImVec2 socket_screen_pos = nlayout.socket_area_pos;

      for (auto&& s : g.output_sockets(n)) {

        // layout
        socket_layout slayout =
          calc_socket_layout(socket_screen_pos, nlayout.size, s, g);

        // move cursor
        socket_screen_pos.y += slayout.size.y;

        nlayout.sockets.emplace(s, std::move(slayout));
      }

      for (auto&& s : g.input_sockets(n)) {

        // layout
        socket_layout slayout =
          calc_socket_layout(socket_screen_pos, nlayout.size, s, g);

        // move cursor
        socket_screen_pos.y += slayout.size.y;

        nlayout.sockets.emplace(s, std::move(slayout));
      }

      layout.map.emplace(n, std::move(nlayout));
    }

    return layout;
  }

} // namespace yave::editor::imgui