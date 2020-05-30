//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/mouse_events.hpp>
#include <yave/wm/key_events.hpp>

namespace yave::wm {

  // fwd
  class window_manager;

  /// Input state machine manager
  class viewport_io_state
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    viewport_io_state(wm::window_manager& mngr);
    /// dtor
    ~viewport_io_state() noexcept;

  public:
    /// register press event
    void register_mouse_press1(uid window_id, wm::mouse_button button);
    /// register press event
    void register_mouse_press2(uid window_id, wm::mouse_button button);
    /// register release event
    void register_mouse_release(uid window_id, wm::mouse_button button);
    /// register hover event
    void register_mouse_hover(uid window_id);

  public:
    /// query new event
    [[nodiscard]] auto query_mouse_click() //
      -> std::vector<wm::mouse_button>;
    /// query new event
    [[nodiscard]] auto query_mouse_double_click() //
      -> std::vector<wm::mouse_button>;
    /// query new event
    [[nodiscard]] auto query_mouse_drag() //
      -> std::vector<wm::mouse_button>;
  };
} // namespace yave::wm