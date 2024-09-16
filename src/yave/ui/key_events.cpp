//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/key_events.hpp>

namespace yave::ui::events {

  key_event::key_event(const window& target, event_phase phase, ui::key key)
    : event(target, phase)
    , m_key {key}
  {
  }

  key_press::key_press(
    const window& target,
    event_phase phase,
    ui::key key,
    ui::key_action action,
    ui::key_modifiers mods)
    : key_event(target, phase, key)
    , m_action {action}
    , m_mods {mods}
  {
  }

  auto key_press::key() const -> ui::key
  {
    return m_key;
  }

  auto key_press::modifiers() const -> ui::key_modifiers
  {
    return m_mods;
  }

  auto key_press::action() const -> ui::key_action
  {
    return m_action;
  }

  bool key_press::is_repeat() const
  {
    return m_action == ui::key_action::repeat;
  }

  bool key_press::test_modifiers(ui::key_modifiers mods) const
  {
    return static_cast<bool>(m_mods & mods);
  }

  bool key_press::shift() const
  {
    return test_modifiers(ui::key_modifiers::shift);
  }

  bool key_press::control() const
  {
    return test_modifiers(ui::key_modifiers::control);
  }

  bool key_press::alt() const
  {
    return test_modifiers(ui::key_modifiers::alt);
  }

  bool key_press::super() const
  {
    return test_modifiers(ui::key_modifiers::super);
  }

  bool key_press::caps_lock() const
  {
    return test_modifiers(ui::key_modifiers::caps_lock);
  }

  bool key_press::num_lock() const
  {
    return test_modifiers(ui::key_modifiers::num_lock);
  }

  key_release::key_release(const window& target, event_phase phase, ui::key key)
    : key_event(target, phase, key)
  {
  }

  auto key_release::key() const -> ui::key
  {
    return m_key;
  }

  key_char::key_char(const window& target, event_phase phase, std::u8string str)
    : key_event(target, phase, key::unknown)
    , m_str {str}
  {
  }

  auto key_char::chars() const -> std::u8string_view
  {
    return m_str;
  }
} // namespace yave::ui::events