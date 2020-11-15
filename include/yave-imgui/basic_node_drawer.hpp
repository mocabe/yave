//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave-imgui/node_window_drawables.hpp>

namespace yave::editor::imgui {

  /// calc minimum size of node header with text
  [[nodiscard]] ImVec2 calc_node_header_min_size(const std::string& title);

  /// Generic node drawer
  class basic_node_drawer : public node_drawable
  {
    struct socket_area
    {
      socket_handle socket;
      ImVec2 pos, size;
    };

    // title
    std::string m_title;
    // pos
    glm::vec2 m_pos;

    // cache socket layout
    mutable bool m_layout_cache = false;
    mutable ImVec2 m_cached_header_size;
    mutable ImVec2 m_cached_node_size;
    mutable std::vector<socket_area> m_cached_areas;

  public:
    basic_node_drawer(
      const node_handle& n,
      const structured_node_graph& g,
      const node_window& nw);

    auto screen_pos(const node_window& nw, node_window_draw_info& draw_info)
      const -> ImVec2 override;

    auto size(node_window_draw_info& draw_info) const -> ImVec2 override;

    auto socket_area_pos(
      node_window_draw_info& draw_info,
      const socket_handle& s) const -> ImVec2 override;

    auto socket_area_size(
      node_window_draw_info& draw_info,
      const socket_handle& s) const -> ImVec2 override;

    void draw(
      const node_window& nw,
      const data_context& dctx,
      const view_context& vctx,
      node_window_draw_info& draw_info,
      ImDrawListSplitter& splitter,
      size_t channel) const override;

  private:
    // calc node size and socket areas
    auto _calc_area_layout(node_window_draw_info& draw_info) const;
    // draw node bg
    void _draw_background(
      bool hovered,
      bool selected,
      const ImVec2& pos,
      const ImVec2& size) const;
    // draw node header
    void _draw_header(
      bool hovered,
      bool selected,
      const ImVec2& pos,
      const ImVec2& size) const;
    // draw edge
    void _draw_edge(
      bool hovered,
      bool selected,
      const ImVec2& pos,
      const ImVec2& size) const;
    // draw popup
    void _draw_popup(
      node_window_draw_info& draw_info,
      const data_context& dctx,
      const view_context& vctx) const;
    // input handling
    void _handle_input(
      const node_window& nw,
      const data_context& dctx,
      const view_context& vctx,
      bool hovered,
      bool selected,
      const ImVec2& pos,
      const ImVec2& size) const;
  };
} // namespace yave::editor::imgui