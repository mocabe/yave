//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/event.hpp>
#include <yave/ui/vec.hpp>

#include <GLFW/glfw3.h>
#include <algorithm>

namespace yave::ui {

  /// mouse button enum
  enum class mouse_button : int
  {
    _1     = GLFW_MOUSE_BUTTON_1,
    _2     = GLFW_MOUSE_BUTTON_2,
    _3     = GLFW_MOUSE_BUTTON_3,
    _4     = GLFW_MOUSE_BUTTON_4,
    _5     = GLFW_MOUSE_BUTTON_5,
    _6     = GLFW_MOUSE_BUTTON_6,
    _7     = GLFW_MOUSE_BUTTON_7,
    _8     = GLFW_MOUSE_BUTTON_8,
    left   = GLFW_MOUSE_BUTTON_LEFT,
    right  = GLFW_MOUSE_BUTTON_RIGHT,
    middle = GLFW_MOUSE_BUTTON_MIDDLE,
  };

  /// mouse button state
  enum class button_state
  {
    up   = GLFW_RELEASE,
    down = GLFW_PRESS,
  };

  /// raw mouse button event
  enum class button_action
  {
    release = GLFW_RELEASE,
    press   = GLFW_PRESS,
  };

  namespace events {

    class mouse_event : public event
    {
    public:
      mouse_event(
        const window& target,
        event_phase phase,
        mouse_button button,
        vec pos);

      /// position of mouse in window coordinate
      auto pos() const -> vec;

      /// event source button
      auto button() const -> mouse_button;

    protected:
      mouse_button m_button;
      vec m_pos;
    };

    /// click event
    /// \note propagate = yes
    class mouse_click final : public mouse_event
    {
    public:
      mouse_click(
        const window& target,
        event_phase phase,
        mouse_button button,
        vec pos);
    };

    /// double click event
    /// \note propagate = yes
    class mouse_double_click final : public mouse_event
    {
    public:
      mouse_double_click(
        const window& target,
        event_phase phase,
        mouse_button button,
        vec pos);
    };

    /// mouse press event
    /// \note propagate = yes
    class mouse_press final : public mouse_event
    {
    public:
      mouse_press(
        const window& target,
        event_phase phase,
        mouse_button button,
        vec pos);
    };

    /// mouse release event
    /// \note propagate = yes
    class mouse_release final : public mouse_event
    {
    public:
      mouse_release(
        const window& target,
        event_phase phase,
        mouse_button button,
        vec pos);
    };

    /// mouse repeat event
    /// \note propagate = yes
    class mouse_repeat final : public mouse_event
    {
    public:
      mouse_repeat(
        const window& target,
        event_phase phase,
        mouse_button button,
        vec pos);
    };

    /// mouse moves over viewport
    /// \note propagate: yes
    class mouse_move final : public mouse_event
    {
      vec m_delta;

    public:
      mouse_move(const window& target, event_phase phase, vec pos, vec delta);

      /// delta
      auto delta() const -> vec;
    };

    /// mouse move into window or its child
    /// \note propagate: yes
    class mouse_over final : public mouse_event
    {
    public:
      mouse_over(const window& target, event_phase phase, vec pos);
    };

    /// mouse move out from window or its child
    /// \note propagate: yes
    class mouse_out final : public mouse_event
    {
    public:
      mouse_out(const window& target, event_phase phase, vec pos);
    };

    /// mosue move into region of window
    /// \note propagate: no
    class mouse_enter final : public mouse_event
    {
      using event::set_accepted;
      using event::accept;
      using event::ignore;
      using event::accepted;

    public:
      mouse_enter(const window& target, event_phase phase, vec pos);
    };

    /// mosue move out from region of window
    /// \note propagate: no
    class mouse_leave final : public mouse_event
    {
      using event::set_accepted;
      using event::accept;
      using event::ignore;
      using event::accepted;

    public:
      mouse_leave(const window& target, event_phase phase, vec pos);
    };

  } // namespace events

} // namespace yave::ui