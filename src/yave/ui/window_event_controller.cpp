//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/window_event_controller.hpp>

namespace yave::ui::controllers {

  window::window()
    : controllerT<window>(event_phase::bubble)
  {
  }

  bool window::event(ui::event& e, view_context& vctx)
  {
    if (e.phase() == phase()) {
      if (e.is<events::window_event>()) {
        /*  */ if (auto w = e.get_as<events::show>()) {
          return event(*w, vctx);
        } else if (auto w = e.get_as<events::hide>()) {
          return event(*w, vctx);
        } else if (auto w = e.get_as<events::close>()) {
          return event(*w, vctx);
        }
      }
    }
    return false;
  }

  bool window::event(events::show& e, view_context& vctx)
  {
    assert(&e.target() == &controller::window());
    e.accept();
    signals.on_show(vctx);
    return true;
  }

  bool window::event(events::hide& e, view_context& vctx)
  {
    assert(&e.target() == &controller::window());
    e.accept();
    signals.on_show(vctx);
    return true;
  }

  bool window::event(events::close& e, view_context& vctx)
  {
    assert(&e.target() == &controller::window());
    e.accept();
    signals.on_close(vctx);
    return true;
  }
}