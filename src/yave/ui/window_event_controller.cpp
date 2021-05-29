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

  bool window_event_controller::event(ui::event& e, view_context& vctx)
  {
    if (e.phase() == phase()) {
      if (e.is<events::window_event>()) {
        if (auto w = e.get_as<events::show>())
          return event(*w, vctx);
        if (auto w = e.get_as<events::hide>())
          return event(*w, vctx);
        if (auto w = e.get_as<events::close>())
          return event(*w, vctx);
      }
    }
    return false;
  }

  bool window_event_controller::event(events::show& e, view_context& vctx)
  {
    assert(&e.target() == &controller::window());
    e.accept();
    signals.on_show(e, vctx);
    return true;
  }

  bool window_event_controller::event(events::hide& e, view_context& vctx)
  {
    assert(&e.target() == &controller::window());
    e.accept();
    signals.on_hide(e, vctx);
    return true;
  }

  bool window_event_controller::event(events::close& e, view_context& vctx)
  {
    assert(&e.target() == &controller::window());
    e.accept();
    signals.on_close(e, vctx);
    return true;
  }
} // namespace yave::ui