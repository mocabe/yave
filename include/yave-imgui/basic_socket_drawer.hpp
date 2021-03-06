//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave-imgui/node_window_drawables.hpp>

namespace yave::editor::imgui {

  class basic_socket_drawer : public socket_drawable
  {
  public:
    basic_socket_drawer(
      const socket_handle& s,
      const structured_node_graph& g,
      const node_window& nw);

    auto min_size(node_window_draw_info&) const -> ImVec2 override;

    auto slot_pos(node_window_draw_info& draw_info, ImVec2 size) const
      -> ImVec2 override;

    void draw(
      const node_window& nw,
      const data_context& dctx,
      const view_context& vctx,
      node_window_draw_info& draw_info,
      ImDrawListSplitter& splitter,
      size_t channel,
      ImVec2 pos,
      ImVec2 size) const override;

  protected:
    /// Draw slot
    virtual void _draw_slot(
      const node_window& nw,
      const data_context& dctx,
      const view_context& vctx,
      node_window_draw_info& draw_info,
      const ImVec2& slot_pos) const;

    /// Draw socket content
    virtual void _draw_content(
      const node_window& nw,
      const data_context& dctx,
      const view_context& vctx,
      node_window_draw_info& draw_info,
      ImVec2 pos,
      ImVec2 size) const;

  private:
    auto _choose_slot_color(
      const node_window& nw,
      const data_context& dctx,
      const view_context& vctx) const -> ImColor;
  };
} // namespace yave::editor::imgui