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

  bool mouse_event_controller::event(ui::event& e, view_context& vctx)
  {
    if (e.phase() == phase()) {
      if (e.is<events::mouse_event>()) {
        if (auto m = e.get_as<events::mouse_click>())
          return event(*m, vctx);
        if (auto m = e.get_as<events::mouse_double_click>())
          return event(*m, vctx);
        if (auto m = e.get_as<events::mouse_press>())
          return event(*m, vctx);
        if (auto m = e.get_as<events::mouse_release>())
          return event(*m, vctx);
        if (auto m = e.get_as<events::mouse_repeat>())
          return event(*m, vctx);
        if (auto m = e.get_as<events::mouse_move>())
          return event(*m, vctx);
        if (auto m = e.get_as<events::mouse_over>())
          return event(*m, vctx);
        if (auto m = e.get_as<events::mouse_out>())
          return event(*m, vctx);
        if (auto m = e.get_as<events::mouse_enter>())
          return event(*m, vctx);
        if (auto m = e.get_as<events::mouse_leave>())
          return event(*m, vctx);
      }
    }
    return false;
  }

  bool mouse_event_controller::event(events::mouse_click& e, view_context& vctx)
  {
    e.accept();
    signals.on_mouse_click(e, vctx);
    return true;
  }

  bool mouse_event_controller::event(
    events::mouse_double_click& e,
    view_context& vctx)
  {
    e.accept();
    signals.on_mouse_double_click(e, vctx);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_press& e, view_context& vctx)
  {
    e.accept();
    signals.on_mouse_press(e, vctx);
    return true;
  }

  bool mouse_event_controller::event(
    events::mouse_release& e,
    view_context& vctx)
  {
    e.accept();
    signals.on_mouse_release(e, vctx);
    return true;
  }

  bool mouse_event_controller::event(
    events::mouse_repeat& e,
    view_context& vctx)
  {
    e.accept();
    signals.on_mouse_repeat(e, vctx);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_move& e, view_context& vctx)
  {
    e.accept();
    signals.on_mouse_move(e, vctx);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_over& e, view_context& vctx)
  {
    e.accept();
    signals.on_mouse_over(e, vctx);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_out& e, view_context& vctx)
  {
    e.accept();
    signals.on_mouse_out(e, vctx);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_enter& e, view_context& vctx)
  {
    e.accept();
    signals.on_mouse_enter(e, vctx);
    return true;
  }

  bool mouse_event_controller::event(events::mouse_leave& e, view_context& vctx)
  {
    e.accept();
    signals.on_mouse_leave(e, vctx);
    return true;
  }

} // namespace yave::ui