//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/node_info.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave-imgui/editor/input_state.hpp>

namespace yave::editor::imgui {

  // fwd
  class editor_context;
  struct socket_view;
  struct connection_view;

  /// Node view interface
  struct node_view
  {
    /// ctor
    node_view(const node_handle& h, const editor_context& ctx);

    /// handle
    node_handle handle;
    /// info
    node_info info;

    /// calc minimum size to render
    virtual auto min_size() const -> ImVec2 = 0;
    /// calc socket slot position
    /// \param socket socket handle
    /// \param pos base position to render
    /// \param size render size
    virtual auto slot_pos(
      const socket_handle& socket,
      const ImVec2& pos,
      const ImVec2& size) const -> ImVec2 = 0;
    /// draw node.
    /// \param pos base position to render
    /// \param size render size
    virtual void draw(const ImVec2& pos, const ImVec2& size) const = 0;
    /// handle input.
    /// \param pos base position to render
    /// \param size render size
    /// \param state input state manager
    virtual void handle_input(
      const ImVec2& base,
      const ImVec2& size,
      input_state& state) const = 0;
    /// dtor
    virtual ~node_view() noexcept = 0;
  };

  /// Node view
  struct basic_node_view : node_view
  {
  public:
    basic_node_view(
      const node_handle& h,
      const editor_context& ctx,
      const std::vector<std::shared_ptr<socket_view>>& sockets);

    /// selected?
    bool is_selected;

    /// hovered?
    bool is_hovered;

    /// position
    ImVec2 position;

    /// header title
    std::string title;

    /// sockets
    std::vector<std::shared_ptr<socket_view>> sockets;

  public:
    auto min_size() const -> ImVec2 override;
    void draw(const ImVec2& base, const ImVec2& size) const override;
    auto slot_pos(
      const socket_handle& socket,
      const ImVec2& pos,
      const ImVec2& size) const -> ImVec2 override;
    void handle_input(
      const ImVec2& base,
      const ImVec2& size,
      input_state& state) const override;
  };

} // namespace yave::editor::imgui