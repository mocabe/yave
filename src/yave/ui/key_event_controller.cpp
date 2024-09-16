//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/key_event_controller.hpp>

namespace yave::ui {

  key_event_controller::key_event_controller(event_phase phase)
    : controllerT<key_event_controller>(phase)
  {
  }

  bool key_event_controller::event(ui::event& e)
  {
    if (e.phase() == phase()) {
      // Keyboard events
      if (e.is<events::key_event>()) {
        if (auto k = e.get_as<events::key_press>())
          return event(*k);
        if (auto k = e.get_as<events::key_release>())
          return event(*k);
        if (auto k = e.get_as<events::key_char>())
          return event(*k);
      }
      // Keyboard focus events
      if (e.is<events::focus_event>()) {
        if (auto f = e.get_as<events::focusing>())
          return event(*f);
        if (auto f = e.get_as<events::blurring>())
          return event(*f);
        if (auto f = e.get_as<events::focus>())
          return event(*f);
        if (auto f = e.get_as<events::blur>())
          return event(*f);
      }
    }
    return false;
  }

  bool key_event_controller::event(events::key_press& e)
  {
    e.accept();
    signals.on_key_press(e);
    return true;
  }

  bool key_event_controller::event(events::key_release& e)
  {
    e.accept();
    signals.on_key_release(e);
    return true;
  }

  bool key_event_controller::event(events::key_char& e)
  {
    e.accept();
    signals.on_key_char(e);
    return true;
  }

  bool key_event_controller::event(events::focusing& e)
  {
    e.accept();
    signals.on_focusing(e);
    return true;
  }

  bool key_event_controller::event(events::blurring& e)
  {
    e.accept();
    signals.on_blurring(e);
    return true;
  }

  bool key_event_controller::event(events::focus& e)
  {
    signals.on_focus(e);
    return true;
  }

  bool key_event_controller::event(events::blur& e)
  {
    signals.on_blur(e);
    return true;
  }

} // namespace yave::ui
