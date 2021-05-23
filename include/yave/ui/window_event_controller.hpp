//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/window_events.hpp>
#include <yave/ui/controller.hpp>
#include <yave/ui/signal.hpp>

namespace yave::ui::controllers {

  class window final : public generic_controller<window>
  {
  public:
    window();

    bool event(ui::event& e, view_context& vctx) override;

    virtual bool event(events::show& e, view_context& vctx);
    virtual bool event(events::hide& e, view_context& vctx);
    virtual bool event(events::close& e, view_context& vctx);

    struct _signals
    {
      signal<view_context&> on_show;
      signal<view_context&> on_hide;
      signal<view_context&> on_close;
    } signals;
  };

} // namespace yave::ui::controllers