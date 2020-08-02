//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/editor/data_context.hpp>
#include <yave/editor/view_context.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/lib/imgui/extension.hpp>

#include <yave-imgui/layout.hpp>
#include <range/v3/view.hpp>

namespace yave::editor::imgui {

  // fwd
  class node_window;
  struct node_window_draw_info;

  struct socket_drawable
  {
    /// handle
    socket_handle handle;
    /// info
    structured_socket_info info;

    socket_drawable(const socket_handle& s, const structured_socket_info& i)
      : handle {s}
      , info {i}
    {
    }

    virtual ~socket_drawable() noexcept = default;

    /// relative minimum size of this socket
    virtual auto min_size(node_window_draw_info&) const -> ImVec2 = 0;

    /// relative slot pos
    /// \param size actual socket area size
    virtual auto slot_pos(node_window_draw_info&, ImVec2 size) const
      -> ImVec2 = 0;

    /// draw socket
    /// \param pos area pos to draw
    /// \param size area size to draw
    virtual void draw(
      const node_window& nw,
      const data_context& dctx,
      const view_context& vctx,
      node_window_draw_info& info,
      ImDrawListSplitter& splitter,
      size_t channel,
      ImVec2 pos,
      ImVec2 size) const = 0;
  };

  struct node_drawable
  {
    /// get handle
    node_handle handle;
    /// info
    structured_node_info info;

    node_drawable(const node_handle& n, const structured_node_info& i)
      : handle {n}
      , info {i}
    {
    }

    virtual ~node_drawable() noexcept = default;

    /// get screen pos
    virtual auto screen_pos(
      const node_window& nw,
      node_window_draw_info& draw_info) const -> ImVec2 = 0;

    /// get size
    virtual auto size(node_window_draw_info& draw_info) const -> ImVec2 = 0;

    /// get socket area pos
    virtual auto socket_area_pos(
      node_window_draw_info& draw_info,
      const socket_handle& s) const -> ImVec2 = 0;

    /// get socket area size
    virtual auto socket_area_size(
      node_window_draw_info& draw_info,
      const socket_handle& s) const -> ImVec2 = 0;

    /// draw node
    virtual void draw(
      const node_window& nw,
      const data_context& dctx,
      const view_context& vctx,
      node_window_draw_info& draw_info,
      ImDrawListSplitter& splitter,
      size_t channel) const = 0;
  };

  struct connection_drawable
  {
    /// handle
    connection_handle handle;
    /// info
    structured_connection_info info;

    connection_drawable(
      const connection_handle& c,
      const structured_connection_info& i)
      : handle {c}
      , info {i}
    {
    }

    virtual ~connection_drawable() noexcept = default;

    /// draw connection
    virtual void draw(
      const node_window& nw,
      const data_context& dctx,
      const view_context& vctx,
      node_window_draw_info& draw_info,
      ImDrawListSplitter& splitter,
      int channel) = 0;
  };

  /// create drawable
  [[nodiscard]] auto create_node_drawable(
    const node_handle& n,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<node_drawable>;

  /// create drawable
  [[nodiscard]] auto create_socket_drawable(
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<socket_drawable>;

  /// create drawable
  [[nodiscard]] auto create_connection_drawable(
    const connection_handle& c,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<connection_drawable>;

} // namespace yave::editor::imgui