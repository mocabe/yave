//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/mouse_event_controller.hpp>

namespace yave::ui {

  mouse_event_controller::mouse_event_controller(event_phase phase)
    : controllerT<mouse_event_controller>(phase)
  {
  }

  bool mouse_event_controller::event(ui::event& e)
  {
    if (e.phase() == phase()) {
      if (e.is<events::mouse_event>()) {
        if (auto m = e.get_as<events::mouse_click>())
          return event(*m);
        if (auto m = e.get_as<events::mouse_double_click>())
          return event(*m);
        if (auto m = e.get_as<events::mouse_press>())
          return event(*m);
        if (auto m = e.get_as<events::mouse_release>())
          return event(*m);
        if (auto m = e.get_as<events::mouse_repeat>())
          return event(*m);
        if (auto m = e.get_as<events::mouse_move>())
          return event(*m);
        if (auto m = e.get_as<events::mouse_over>())
          return event(*m);
        if (auto m = e.get_as<events::mouse_out>())
          return event(*m);
        if (auto m = e.get_as<events::mouse_enter>())
          return event(*m);
        if (auto m = e.get_as<events::mouse_leave>())
          return event(*m);
      }
    }
    return false;
  }

  bool mouse_event_controller::event(events::mouse_click& e)
  {
    e.accept();
    signals.on_mouse_click(e);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_double_click& e)
  {
    e.accept();
    signals.on_mouse_double_click(e);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_press& e)
  {
    e.accept();
    signals.on_mouse_press(e);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_release& e)
  {
    e.accept();
    signals.on_mouse_release(e);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_repeat& e)
  {
    e.accept();
    signals.on_mouse_repeat(e);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_move& e)
  {
    e.accept();
    signals.on_mouse_move(e);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_over& e)
  {
    e.accept();
    signals.on_mouse_over(e);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_out& e)
  {
    e.accept();
    signals.on_mouse_out(e);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_enter& e)
  {
    signals.on_mouse_enter(e);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_leave& e)
  {
    signals.on_mouse_leave(e);
    return true;
  }

} // namespace yave::ui