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

    bool event(ui::event& e) override;

    virtual bool event(events::show& e);
    virtual bool event(events::hide& e);
    virtual bool event(events::close& e);

    struct _signals
    {
      signal<events::show&> on_show;
      signal<events::hide&> on_hide;
      signal<events::close&> on_close;
    } signals;
  };

} // namespace yave::ui::controllers