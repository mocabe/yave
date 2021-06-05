//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/mouse_events.hpp>
#include <yave/ui/controller.hpp>

namespace yave::ui {

  /// Mouse event controller
  class mouse_event_controller : public controllerT<mouse_event_controller>
  {
  public:
    mouse_event_controller(event_phase phase);

  public:
    bool event(ui::event& e) override;

    using controller::set_phase;

    virtual bool event(events::mouse_click& e);
    virtual bool event(events::mouse_double_click& e);
    virtual bool event(events::mouse_press& e);
    virtual bool event(events::mouse_release& e);
    virtual bool event(events::mouse_repeat& e);
    virtual bool event(events::mouse_move& e);
    virtual bool event(events::mouse_over& e);
    virtual bool event(events::mouse_out& e);
    virtual bool event(events::mouse_enter& e);
    virtual bool event(events::mouse_leave& e);

    struct _signals
    {
      // clang-format off
      signal<events::mouse_click&>        on_mouse_click;
      signal<events::mouse_double_click&> on_mouse_double_click;
      signal<events::mouse_press&>        on_mouse_press;
      signal<events::mouse_release&>      on_mouse_release;
      signal<events::mouse_repeat&>       on_mouse_repeat;
      signal<events::mouse_move&>         on_mouse_move;
      signal<events::mouse_over&>         on_mouse_over;
      signal<events::mouse_out&>          on_mouse_out;
      signal<events::mouse_enter&>        on_mouse_enter;
      signal<events::mouse_leave&>        on_mouse_leave;
      // clang-format on
    } signals;
  };

} // namespace yave::ui