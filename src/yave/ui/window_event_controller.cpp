//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/window_event_controller.hpp>

namespace yave::ui {

  window_event_controller::window_event_controller()
    : controllerT<window_event_controller>(event_phase::bubble)
  {
  }

  bool window_event_controller::event(ui::event& e)
  {
    if (e.phase() == phase()) {
      if (e.is<events::window_event>()) {
        if (auto w = e.get_as<events::show>())
          return event(*w);
        if (auto w = e.get_as<events::hide>())
          return event(*w);
        if (auto w = e.get_as<events::close>())
          return event(*w);
      }
    }
    return false;
  }

  bool window_event_controller::event(events::show& e)
  {
    assert(&e.target() == &controller::window());
    signals.on_show(e);
    return true;
  }

  bool window_event_controller::event(events::hide& e)
  {
    assert(&e.target() == &controller::window());
    signals.on_hide(e);
    return true;
  }

  bool window_event_controller::event(events::close& e)
  {
    assert(&e.target() == &controller::window());
    e.accept();
    signals.on_close(e);
    return true;
  }
} // namespace yave::ui