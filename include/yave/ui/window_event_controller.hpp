//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/window_events.hpp>
#include <yave/ui/controller.hpp>
#include <yave/ui/signal.hpp>

namespace yave::ui {

  class window_event_controller : public controllerT<window_event_controller>
  {
  public:
    window_event_controller();

    bool event(ui::event& e, view_context& vctx) override;

    virtual bool event(events::show& e, view_context& vctx);
    virtual bool event(events::hide& e, view_context& vctx);
    virtual bool event(events::close& e, view_context& vctx);

    struct _signals
    {
      signal<events::show&, view_context&> on_show;
      signal<events::hide&, view_context&> on_hide;
      signal<events::close&, view_context&> on_close;
    } signals;
  };

} // namespace yave::ui::controllers