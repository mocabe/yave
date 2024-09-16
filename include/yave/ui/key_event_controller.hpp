//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/key_events.hpp>
#include <yave/ui/focus_events.hpp>
#include <yave/ui/controller.hpp>
#include <yave/ui/typeid_cast.hpp>

namespace yave::ui {

  /// Key event controller
  class key_event_controller : public controllerT<key_event_controller>
  {
  public:
    key_event_controller(event_phase phase);

    /// Set event phase
    using controller::set_phase;

    bool event(ui::event& e) override;

    virtual bool event(events::key_press& e);
    virtual bool event(events::key_release& e);
    virtual bool event(events::key_char& e);
    virtual bool event(events::focusing& e);
    virtual bool event(events::blurring& e);
    virtual bool event(events::focus& e);
    virtual bool event(events::blur& e);

    struct _signals
    {
      signal<const events::key_press&> on_key_press;
      signal<const events::key_release&> on_key_release;
      signal<const events::key_char&> on_key_char;
      signal<const events::focusing&> on_focusing;
      signal<const events::blurring&> on_blurring;
      signal<const events::focus&> on_focus;
      signal<const events::blur&> on_blur;
    } signals;
  };

} // namespace yave::ui
