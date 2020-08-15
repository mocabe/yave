//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/key_events.hpp>
#include <yave/wm/mouse_events.hpp>

#include <yave/lib/glfw/glfw_context.hpp>
#include <vector>
#include <string>

namespace yave::wm {

  /// io state of viewport
  class viewport_io
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// Setup event handler for window.
    /// Window must outlive this class.
    viewport_io(glfw::glfw_window& win);
    /// dtor
    ~viewport_io() noexcept;

  public:
    /// get window handle
    auto window() -> glfw::glfw_window&;

  public:
    /// Call glfw_context::poll_events() and update button states
    void update();

  public:
    /// Get current key state
    [[nodiscard]] auto get_key_state(wm::key k) const -> key_state;
    /// Get current mouse state
    [[nodiscard]] auto get_mouse_state(wm::mouse_button b) const
      -> mouse_button_state;
    /// Get text input
    [[nodiscard]] auto get_text_input() const -> std::u8string;

  public:
    /// Get current mouse position
    [[nodiscard]] auto mouse_pos() const -> fvec2;
    /// Get mouse delta
    [[nodiscard]] auto mouse_delta() const -> fvec2;
  };

} // namespace yave::wm