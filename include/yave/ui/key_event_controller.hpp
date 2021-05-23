//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/key_events.hpp>
#include <yave/ui/focus_events.hpp>
#include <yave/ui/controller.hpp>
#include <yave/ui/typeid_cast.hpp>

namespace yave::ui::controllers {

  /// Key event controller
  class key : public controller
  {
  public:
    key(event_phase phase);

    /// Set event phase
    using controller::set_phase;

    bool event(ui::event& e, view_context& vctx) override;

    virtual bool event(events::key_press& e, view_context& vctx);
    virtual bool event(events::key_release& e, view_context& vctx);
    virtual bool event(events::key_char& e, view_context& vctx);
    virtual bool event(events::focusing& e, view_context& vctx);
    virtual bool event(events::blurring& e, view_context& vctx);
    virtual bool event(events::focus& e, view_context& vctx);
    virtual bool event(events::blur& e, view_context& vctx);

    struct _signals
    {
      signal<const events::key_press&, view_context&> on_key_press;
      signal<const events::key_release&, view_context&> on_key_release;
      signal<const events::key_char&, view_context&> on_key_char;
      signal<view_context&> on_focusing;
      signal<view_context&> on_blurring;
      signal<view_context&> on_focus;
      signal<view_context&> on_blur;
    } signals;
  };

} // namespace yave::ui::controllers
