//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/basic_node_drawer.hpp>
#include <yave-imgui/node_window.hpp>

#include <yave/editor/editor_data.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <imgui_stdlib.h>
#include <iostream>

namespace yave::editor::imgui {

  using namespace yave::imgui;

  /// minimum size of node header
  auto calc_node_header_min_size(const std::string& title) -> ImVec2
  {
    auto text_size  = calc_text_size(title, font_size_level::e15);
    auto min_width  = 144.f;
    auto min_height = 32.f;
    auto x_padding  = 32.f;
    auto y_padding  = 0.f;
    return {
      std::max(text_size.x + x_padding * 2, min_width),
      std::max(text_size.y + y_padding * 2, min_height)};
  }

  basic_node_drawer::basic_node_drawer(
    const node_handle& n,
    const structured_node_graph& g,
    const node_window& nw)
    : node_drawable {n, *g.get_info(n)}
  {
  }

  auto basic_node_drawer::_calc_area_layout(
    node_window_draw_info& draw_info) const
  {
    // cached
    if (m_layout_cache)
      return std::make_tuple(
        m_cached_header_size, m_cached_node_size, m_cached_areas);

    // header min size
    auto header_min_size = calc_node_header_min_size(info.name());
    // total node size
    auto node_size = header_min_size;
    // socket area list
    auto socket_areas = std::vector<socket_area>();
    {
      for (auto&& s : info.output_sockets()) {
        // minimum socket size
        auto minsz = draw_info.find_drawable(s)->min_size(draw_info);
        // add socket area
        socket_areas.push_back({s, {0, node_size.y}, minsz});
        // update size
        node_size = {std::max(node_size.x, minsz.x), node_size.y + minsz.y};
      }

      for (auto&& s : info.input_sockets()) {
        auto minsz = draw_info.find_drawable(s)->min_size(draw_info);
        socket_areas.push_back({s, {0, node_size.y}, minsz});
        node_size = {std::max(node_size.x, minsz.x), node_size.y + minsz.y};
      }
    }

    // match socket width to node width
    for (auto&& area : socket_areas) {
      area.size.x = node_size.x;
    }

    // calc header size
    auto header_size = node_size;
    header_size.y    = header_min_size.y;

    // cache result
    m_cached_header_size = header_size;
    m_cached_node_size   = node_size;
    m_cached_areas       = socket_areas;
    m_layout_cache       = true;

    return std::make_tuple(header_size, node_size, socket_areas);
  }

  void basic_node_drawer::_draw_background(
    bool hovered,
    bool selected,
    const ImVec2& pos,
    const ImVec2& size) const
  {
    auto fill = hovered ? get_node_background_fill_color_hovered()
                        : selected ? get_node_background_fill_color_selected()
                                   : get_node_background_fill_color();

    auto rounding  = get_node_rounding();
    auto draw_list = ImGui::GetWindowDrawList();

    // fill
    draw_list->AddRectFilled(pos, pos + size, fill, rounding);
  }

  void basic_node_drawer::_draw_header(
    bool hovered,
    bool selected,
    const ImVec2& pos,
    const ImVec2& size) const
  {
    auto fill = hovered ? get_node_header_fill_color_hovered()
                        : selected ? get_node_header_fill_color_selected()
                                   : get_node_header_fill_color();

    auto title     = info.name();
    auto rounding  = get_node_rounding();
    auto draw_list = ImGui::GetWindowDrawList();

    // fill
    draw_list->AddRectFilled(
      pos, {pos.x + size.x, pos.y + size.y / 2}, fill, rounding);
    draw_list->AddRectFilled(
      {pos.x, pos.y + size.y / 2 - 10}, pos + size, fill, 0);

    // text
    auto text_pos =
      calc_text_pos(title, font_size_level::e15, size, text_alignment::center);
    ImGui::SetCursorScreenPos(pos + text_pos);
    ImGui::TextColored(get_node_header_text_color(), "%s", title.c_str());
  }

  void basic_node_drawer::_draw_edge(
    bool hovered,
    bool selected,
    const ImVec2& pos,
    const ImVec2& size) const
  {
    auto col = hovered ? get_node_edge_color_hovered()
                       : selected ? get_node_edge_color_selected()
                                  : get_node_edge_color();

    auto dl = ImGui::GetWindowDrawList();
    dl->AddRect(pos, pos + size, col, get_node_rounding());
  }

  void basic_node_drawer::_handle_input(
    const node_window& nw,
    data_context& dctx,
    view_context& vctx,
    bool hovered,
    bool selected,
    const ImVec2& pos,
    const ImVec2& size) const
  {
    const auto& n          = handle;
    const char* popup_name = "node popup";

    ImGui::PushID(n.id().data);
    {
      ImGui::SetCursorScreenPos(pos);

      bool hov = false;
      InvisibleButtonEx("node input", size, &hov);

      // neutral
      if (nw.state() == node_window::state::neutral) {

        if (hov) {

          // set hover
          vctx.push(
            make_window_view_command(nw, [n](auto& w) { w.set_hovered(n); }));

          // double click: inspect
          if (info.is_group() && ImGui::IsMouseDoubleClicked(0))
            vctx.push(
              make_window_view_command(nw, [n](auto& w) { w.set_group(n); }));

          // left click: select
          if (ImGui::IsMouseClicked(0) && !ImGui::IsMouseDoubleClicked(0)) {

            auto mpos = to_tvec2(ImGui::GetMousePos());

            if (nw.get_selected_nodes().size() <= 1)
              dctx.exec(make_data_command([n](auto& ctx) {
                auto& g = ctx.data().node_graph;
                g.bring_front(n);
              }));

            vctx.push(make_window_view_command(nw, [n, mpos](auto& w) {
              if (!w.is_selected(n))
                w.clear_selected();
              w.add_selected(n);
              w.begin_node_drag(mpos);
            }));
          }

          // right click: open info popup
          if (ImGui::IsMouseClicked(1)) {
            ImGui::OpenPopup("node_info_popup");
          }
        }

        if (ImGui::BeginPopup(popup_name)) {
          ImGui::Text("Node: %s", info.name().c_str());
          ImGui::Text("id: %s", to_string(n.id()).c_str());
          ImGui::EndPopup();
        }
      }

      // node drag
      if (nw.state() == node_window::state::node) {
        if (ImGui::IsMouseReleased(0)) {
          if (nw.is_selected(n)) {

            auto new_pos = to_tvec2(pos - nw.scroll() - ImGui::GetWindowPos());

            // set new position
            dctx.exec(make_data_command(
              [n, new_pos](auto& ctx) {
                ctx.data().node_graph.set_pos(n, new_pos);
              },
              [n, pos = info.pos()](auto& ctx) {
                ctx.data().node_graph.set_pos(n, pos);
              }));

            // back to neutral
            if (n == nw.get_selected_nodes()[0])
              vctx.push(make_window_view_command(
                nw, [](auto& w) { w.end_node_drag(); }));
          }
        }
      }
    }
    ImGui::PopID();
  }

  auto basic_node_drawer::screen_pos(
    const node_window& nw,
    node_window_draw_info& draw_info) const -> ImVec2
  {
    auto spos = ImGui::GetWindowPos() + nw.scroll() + to_ImVec2(info.pos());

    // set dummy position to nodes being dragged, to avoid submitting
    // position updates every frame
    if (nw.state() == node_window::state::node && nw.is_selected(handle))
      spos = spos + ImGui::GetMousePos() - nw.drag_source();

    return spos;
  }

  auto basic_node_drawer::size(node_window_draw_info& draw_info) const -> ImVec2
  {
    auto [hs, ns, sa] = _calc_area_layout(draw_info);
    (void)hs;
    (void)sa;
    return ns;
  }

  auto basic_node_drawer::socket_area_pos(
    node_window_draw_info& draw_info,
    const socket_handle& s) const -> ImVec2
  {
    auto [hs, ns, sa] = _calc_area_layout(draw_info);
    (void)hs;
    (void)ns;

    for (auto&& [ss, apos, asize] : sa) {
      (void)asize;
      if (s == ss)
        return apos;
    }
    unreachable();
  }

  auto basic_node_drawer::socket_area_size(
    node_window_draw_info& draw_info,
    const socket_handle& s) const -> ImVec2
  {
    auto [hs, ns, sa] = _calc_area_layout(draw_info);
    (void)hs;
    (void)ns;

    for (auto&& [ss, apos, asize] : sa) {
      (void)apos;
      if (s == ss)
        return asize;
    }
    unreachable();
  }

  void basic_node_drawer::draw(
    const node_window& nw,
    data_context& dctx,
    view_context& vctx,
    node_window_draw_info& draw_info,
    ImDrawListSplitter& splitter,
    size_t channel) const
  {
    // layout
    auto [header_size, node_size, socket_areas] = _calc_area_layout(draw_info);

    // draw sockets
    // assume channels are reserved for sockets
    auto socket_ch = channel + 1;

    auto screen_pos = this->screen_pos(nw, draw_info);

    // draw sockets
    for (auto&& [s, apos, asize] : socket_areas) {
      draw_info.find_drawable(s)->draw(
        nw,
        dctx,
        vctx,
        draw_info,
        splitter,
        socket_ch,
        screen_pos + apos,
        asize);

      ++socket_ch;
    }

    bool hov = nw.is_hovered(handle);
    bool sel = nw.is_selected(handle);

    // draw node
    splitter.SetCurrentChannel(ImGui::GetWindowDrawList(), channel);
    _draw_background(hov, sel, screen_pos, node_size);
    _draw_header(hov, sel, screen_pos, header_size);
    _draw_edge(hov, sel, screen_pos, node_size);
    _handle_input(nw, dctx, vctx, hov, sel, screen_pos, node_size);
  }
} // namespace yave::editor::imgui