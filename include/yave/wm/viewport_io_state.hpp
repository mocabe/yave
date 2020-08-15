//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/key_events.hpp>
#include <yave/wm/mouse_events.hpp>

#include <yave/lib/glfw/glfw_context.hpp>
#include <vector>

namespace yave::wm {

  // fwd
  class window_manager;
  class viewport_io;

  /// State machine for high level input events.
  /// Some high level events such as clicks depend on result of low level event
  /// dispatch, so user must register these results for further event
  /// processing.
  class viewport_io_state
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    viewport_io_state(glfw::glfw_window& win, wm::window_manager& mngr);
    /// dtor
    ~viewport_io_state() noexcept;

  public:
    /// update state machine. should be called at beggining of each frame.
    void update();

  public:
    /// query low level key event
    [[nodiscard]] auto query_key_press() const -> std::vector<wm::key>;
    [[nodiscard]] auto query_key_repeat() const -> std::vector<wm::key>;
    [[nodiscard]] auto query_key_release() const -> std::vector<wm::key>;

  public:
    /// query low level mouse event
    [[nodiscard]] auto query_mouse_press() const
      -> std::vector<wm::mouse_button>;
    [[nodiscard]] auto query_mouse_repeat() const
      -> std::vector<wm::mouse_button>;
    [[nodiscard]] auto query_mouse_release() const
      -> std::vector<wm::mouse_button>;

  public:
    /// register result of low level events.
    void register_mouse_press(uid wid, wm::mouse_button button);
    void register_mouse_release(uid wid, wm::mouse_button button);

  public:
    /// query click event
    [[nodiscard]] auto query_mouse_click() //
      -> std::vector<wm::mouse_button>;
    /// query double click event
    [[nodiscard]] auto query_mouse_double_click() //
      -> std::vector<wm::mouse_button>;
    /// query drag event
    [[nodiscard]] auto query_mouse_drag() //
      -> std::vector<wm::mouse_button>;
  };
} // namespace yave::wm