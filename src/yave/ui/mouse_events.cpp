//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/mouse_events.hpp>

namespace yave::ui::events {

  mouse_event::mouse_event(
    const window& target,
    event_phase phase,
    mouse_button button,
    vec pos)
    : event(target, phase)
    , m_button {button}
    , m_pos {pos}
  {
  }

  auto mouse_event::pos() const -> vec
  {
    return m_pos;
  }

  auto mouse_event::button() const -> mouse_button
  {
    return m_button;
  }

  mouse_click::mouse_click(
    const window& target,
    event_phase phase,
    mouse_button button,
    vec pos)
    : mouse_event(target, phase, button, pos)
  {
  }

  mouse_double_click::mouse_double_click(
    const window& target,
    event_phase phase,
    mouse_button button,
    vec pos)
    : mouse_event(target, phase, button, pos)
  {
  }
  mouse_press::mouse_press(
    const window& target,
    event_phase phase,
    mouse_button button,
    vec pos)
    : mouse_event(target, phase, button, pos)
  {
  }
  mouse_release::mouse_release(
    const window& target,
    event_phase phase,
    mouse_button button,
    vec pos)
    : mouse_event(target, phase, button, pos)
  {
  }

  mouse_repeat::mouse_repeat(
    const window& target,
    event_phase phase,
    mouse_button button,
    vec pos)
    : mouse_event(target, phase, button, pos)
  {
  }

  mouse_move::mouse_move(
    const window& target,
    event_phase phase,
    vec pos,
    vec delta)
    : mouse_event(target, phase, mouse_button(), pos)
    , m_delta {delta}
  {
  }

  auto mouse_move::delta() const -> vec
  {
    return m_delta;
  }

  mouse_over::mouse_over(const window& target, event_phase phase, vec pos)
    : mouse_event(target, phase, mouse_button(), pos)
  {
  }

  mouse_out::mouse_out(const window& target, event_phase phase, vec pos)
    : mouse_event(target, phase, mouse_button(), pos)
  {
  }

  mouse_enter::mouse_enter(const window& target, event_phase phase, vec pos)
    : mouse_event(target, phase, mouse_button(), pos)
  {
  }

  mouse_leave::mouse_leave(const window& target, event_phase phase, vec pos)
    : mouse_event(target, phase, mouse_button(), pos)
  {
  }

} // namespace yave::ui::events