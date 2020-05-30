//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/lib/glfw/glfw_context.hpp>

#include <yave/wm/key_events.hpp>
#include <yave/wm/mouse_events.hpp>

#include <vector>

namespace yave::wm {

  /// input state for keys
  struct key_inputs
  {
    // key press
    std::vector<wm::key> press;
    // key release
    std::vector<wm::key> release;
    // key release
    std::vector<wm::key> repeat;
  };

  /// input state for mouse
  struct mouse_inputs
  {
    // first press
    std::vector<wm::mouse_button> press1;
    // double press
    std::vector<wm::mouse_button> press2;
    // release
    std::vector<wm::mouse_button> release;
    // repeat
    std::vector<wm::mouse_button> repeat;
    // cursor pos in virtual screen coordinate
    fvec2 cursor_pos;
    // cursor pos delta
    fvec2 cursor_delta;
    // window hover
    bool hovered;
  };

  /// Handles user input of window
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
    /// Get registered key input
    [[nodiscard]] auto query_key_input() -> key_inputs;
    /// Get registered mouse input
    [[nodiscard]] auto query_mouse_input() -> mouse_inputs;
  };
} // namespace yave::wm
