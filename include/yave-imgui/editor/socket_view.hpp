//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/socket_handle.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave-imgui/editor/input_state.hpp>

namespace yave::editor::imgui {

  /// Socket editor object
  struct socket_view
  {
    /// width of this socket
    virtual auto min_width() const -> float = 0;
    /// height of this socket
    virtual auto height() const -> float = 0;
    /// calc slot position.
    /// \param base cursor pos
    virtual auto slot_pos(const ImVec2& base) const -> ImVec2 = 0;
    /// draw content.
    /// \param base cursor pos
    virtual void draw(const ImVec2& base) const = 0;
    /// handle input.
    /// \param pos cursor pos
    /// \param state input state
    virtual void handle_input(const ImVec2& base, input_state& state) const = 0;
    /// dtor
    virtual ~socket_view() noexcept = 0;
  };

  // fwd
  class editor_context;

  struct basic_socket_view : socket_view
  {
    basic_socket_view(const socket_handle& h, const editor_context& ctx);

    /// handle
    socket_handle handle;

    /// selected?
    bool is_selected;

    /// hovered?
    bool is_hovered;

    /// socket type
    socket_type type;

  public:
    auto min_width() const -> float override;
    auto height() const -> float override;
    auto slot_pos(const ImVec2&) const -> ImVec2 override;
    void draw(const ImVec2& pos) const override;
    void handle_input(const ImVec2&, input_state&) const override;
  };

  /// Create socket view
  [[nodiscard]] auto create_socket_view(
    const socket_handle& socket,
    const editor_context& ctx) -> std::unique_ptr<socket_view>;

} // namespace yave::editor::imgui