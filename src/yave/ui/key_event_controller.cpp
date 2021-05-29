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

  bool key_event_controller::event(ui::event& e, view_context& vctx)
  {
    if (e.phase() == phase()) {
      // Keyboard events
      if (e.is<events::key_event>()) {
        if (auto k = e.get_as<events::key_press>())
          return event(*k, vctx);
        if (auto k = e.get_as<events::key_release>())
          return event(*k, vctx);
        if (auto k = e.get_as<events::key_char>())
          return event(*k, vctx);
      }
      // Keyboard focus events
      if (e.is<events::focus_event>()) {
        if (auto f = e.get_as<events::focusing>())
          return event(*f, vctx);
        if (auto f = e.get_as<events::blurring>())
          return event(*f, vctx);
        if (auto f = e.get_as<events::focus>())
          return event(*f, vctx);
        if (auto f = e.get_as<events::blur>())
          return event(*f, vctx);
      }
    }
    return false;
  }

  bool key_event_controller::event(events::key_press& e, view_context& vctx)
  {
    e.accept();
    signals.on_key_press(e, vctx);
    return true;
  }

  bool key_event_controller::event(events::key_release& e, view_context& vctx)
  {
    e.accept();
    signals.on_key_release(e, vctx);
    return true;
  }

  bool key_event_controller::event(events::key_char& e, view_context& vctx)
  {
    e.accept();
    signals.on_key_char(e, vctx);
    return true;
  }

  bool key_event_controller::event(events::focusing& e, view_context& vctx)
  {
    e.accept();
    signals.on_focusing(e, vctx);
    return true;
  }

  bool key_event_controller::event(events::blurring& e, view_context& vctx)
  {
    e.accept();
    signals.on_blurring(e, vctx);
    return true;
  }

  bool key_event_controller::event(events::focus& e, view_context& vctx)
  {
    e.accept();
    signals.on_focus(e, vctx);
    return true;
  }

  bool key_event_controller::event(events::blur& e, view_context& vctx)
  {
    e.accept();
    signals.on_blur(e, vctx);
    return true;
  }

} // namespace yave::ui
