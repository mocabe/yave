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
    bool event(ui::event& e, view_context& vctx) override;

    using controller::set_phase;

    virtual bool event(events::mouse_click& e, view_context& vctx);
    virtual bool event(events::mouse_double_click& e, view_context& vctx);
    virtual bool event(events::mouse_press& e, view_context& vctx);
    virtual bool event(events::mouse_release& e, view_context& vctx);
    virtual bool event(events::mouse_repeat& e, view_context& vctx);
    virtual bool event(events::mouse_move& e, view_context& vctx);
    virtual bool event(events::mouse_over& e, view_context& vctx);
    virtual bool event(events::mouse_out& e, view_context& vctx);
    virtual bool event(events::mouse_enter& e, view_context& vctx);
    virtual bool event(events::mouse_leave& e, view_context& vctx);

    struct _signals
    {
      // clang-format off
      signal<const events::mouse_click&, view_context&>        on_mouse_click;
      signal<const events::mouse_double_click&, view_context&> on_mouse_double_click;
      signal<const events::mouse_press&, view_context&>        on_mouse_press;
      signal<const events::mouse_release&, view_context&>      on_mouse_release;
      signal<const events::mouse_repeat&, view_context&>       on_mouse_repeat;
      signal<const events::mouse_move&, view_context&>         on_mouse_move;
      signal<const events::mouse_over&, view_context&>         on_mouse_over;
      signal<const events::mouse_out&, view_context&>          on_mouse_out;
      signal<const events::mouse_enter&, view_context&>        on_mouse_enter;
      signal<const events::mouse_leave&, view_context&>        on_mouse_leave;
      // clang-format on
    } signals;
  };

} // namespace yave::ui