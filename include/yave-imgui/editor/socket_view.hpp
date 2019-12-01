//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/socket_handle.hpp>
#include <yave/node/core/socket_info.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave-imgui/editor/input_state.hpp>

namespace yave::editor::imgui {

  // fwd
  class editor_context;


  /// Socket editor object
  struct socket_view
  {
    /// ctor
    socket_view(const socket_handle& h, const editor_context& ctx);

    /// handle
    socket_handle handle;
    /// info
    socket_info info;

    /// calc minimum size of this socket
    virtual auto min_size() const -> ImVec2 = 0;
    /// calc slot position.
    /// \param pos base position to render
    /// \param size render size
    virtual auto slot_pos(const ImVec2& pos, const ImVec2& size) const
      -> ImVec2 = 0;
    /// draw content.
    /// \param pos base position to render
    /// \param size render size
    virtual void draw(const ImVec2& pos, const ImVec2& size) const = 0;
    /// handle input.
    /// \param pos base position to render
    /// \param size render size
    /// \plaram state input state manager
    virtual void handle_input(
      const ImVec2& pos,
      const ImVec2& size,
      input_state& state) const = 0;
    /// dtor
    virtual ~socket_view() noexcept = 0;
  };

  struct basic_socket_view : socket_view
  {
    basic_socket_view(const socket_handle& h, const editor_context& ctx);

    /// selected?
    bool is_selected;

    /// hovered?
    bool is_hovered;

    /// socket type
    socket_type type;

    /// socket name
    std::string name;

  public:
    virtual auto min_size() const -> ImVec2 override;
    virtual auto slot_pos(const ImVec2& base, const ImVec2& size) const
      -> ImVec2 override;
    virtual void draw(const ImVec2& base, const ImVec2& size) const override;
    virtual void handle_input(
      const ImVec2& pos,
      const ImVec2& size,
      input_state& state) const override;
  };

} // namespace yave::editor::imgui