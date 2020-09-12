//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave-imgui/node_window_drawables.hpp>
#include <yave/wm/window.hpp>
#include <yave/editor/data_context.hpp>
#include <yave/editor/view_context.hpp>
#include <yave/lib/imgui/imgui_context.hpp>

namespace yave::editor::imgui {

  /// draw connection line
  void draw_connection_line(
    const ImVec2& p1,
    const ImVec2& p2,
    bool hovered,
    bool selected);

  // draw info
  struct node_window_draw_info
  {
    // nodes
    std::vector<std::unique_ptr<node_drawable>> nodes;
    // sockets
    std::vector<std::unique_ptr<socket_drawable>> sockets;
    // connections
    std::vector<std::unique_ptr<connection_drawable>> connections;

    // total channels
    size_t channel_size;
    // bg
    size_t background_channel_index;
    // connections
    size_t connection_channel_index;
    // base of node/socket channels
    size_t node_channel_index_base;
    // foreground
    size_t foreground_channel_index;

    template <class Handle, class Container>
    auto& _find_drawable(const Handle& h, Container& c) const
    {
      auto it = std::find_if(
        c.begin(), c.end(), [&](auto& p) { return p->handle == h; });
      assert(it != c.end());
      return *it;
    }

    auto& find_drawable(const node_handle& n) const
    {
      return _find_drawable(n, nodes);
    }

    auto& find_drawable(const socket_handle& s) const
    {
      return _find_drawable(s, sockets);
    }

    auto& find_drawable(const connection_handle& c) const
    {
      return _find_drawable(c, connections);
    }
  };

  /// Node editor canvas
  class node_window : public wm::window
  {
    /// imgui context
    yave::imgui::imgui_context& imgui_ctx;

  public:
    enum class state
    {
      neutral,    ///< Neutral state
      background, ///< Dragging background
      node,       ///< Dragging node
      socket,     ///< Dragging socket
    };

  private:
    /// current editor state
    node_window::state current_state;

  private:
    /// selected nodes
    std::vector<node_handle> n_selected;
    /// hovered node
    node_handle n_hovered;

  private:
    /// selected socket
    socket_handle s_selected;

  private:
    /// current group to show
    node_handle current_group;
    /// current group path
    std::string current_group_path;

  private:
    // current scroll position
    glm::fvec2 scroll_pos;
    // last drag source position
    glm::fvec2 drag_source_pos;

  private:
    /// canvas draw info (will be updated every frame)
    std::unique_ptr<node_window_draw_info> draw_info;

  private:
    /// declaration list
    std::vector<std::shared_ptr<node_declaration>> decls;

  public:
    node_window(yave::imgui::imgui_context& im);
    ~node_window() noexcept;

  public:
    void update(editor::data_context& data_ctx, editor::view_context& view_ctx)
      override;

    void draw(
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;

  public: /* state transition functions */
    /// get current state
    auto state() const -> node_window::state;

    /// store pos and handle, transition to background state
    void begin_background_drag(const glm::fvec2& drag_src_pos);
    /// transition to neutral state
    void end_background_drag();

    /// store pos and handle, transition to node state
    void begin_node_drag(const glm::fvec2& drag_src_pos);
    /// transition to neutral state
    void end_node_drag();

    /// store pos and handle, transition to socket state
    void begin_socket_drag(const glm::fvec2& drag_src_pos);
    /// transition to neutral state
    void end_socket_drag();

  public:
    /// get current scroll
    auto scroll() const -> ImVec2;
    /// get drag source pos
    auto drag_source() const -> ImVec2;

  public:
    /// get current group
    auto group() const -> node_handle;
    /// get current group
    void set_group(const node_handle& n);

  public:
    /// hovered?
    bool is_hovered(const node_handle& n) const;
    /// get selected nodes
    auto get_hovered_node() const -> std::optional<node_handle>;
    /// set hover
    void set_hovered(const node_handle& n);
    /// clear hover
    void clear_hovered();

  public:
    /// selected?
    bool is_selected(const node_handle& n) const;
    /// get selected nodes
    auto get_selected_nodes() const -> std::vector<node_handle>;
    /// select
    void add_selected(const node_handle& n);
    /// clear selection
    void clear_selected_nodes();

  public:
    bool is_selected(const socket_handle& s) const;
    auto get_selected_socket() const -> std::optional<socket_handle>;
    void set_selected(const socket_handle& s);
    void clear_selected_socket();

  public:
    /// clear selection
    void clear_selected();

  private:
    /// create new draw info
    auto _create_draw_info(const structured_node_graph& g)
      -> node_window_draw_info;

  private:
    void _draw_background(
      const data_context&,
      const view_context&,
      node_window_draw_info&,
      ImDrawListSplitter&) const;

    void _draw_connections(
      const data_context&,
      const view_context&,
      node_window_draw_info&,
      ImDrawListSplitter&) const;

    void _draw_nodes(
      const data_context&,
      const view_context&,
      node_window_draw_info&,
      ImDrawListSplitter&) const;

    void _draw_foreground(
      const data_context&,
      const view_context&,
      node_window_draw_info&,
      ImDrawListSplitter&) const;
  };
} // namespace yave::editor::imgui