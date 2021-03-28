//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/event.hpp>
#include <yave/ui/controller.hpp>
#include <yave/ui/signal.hpp>
#include <yave/ui/typeid_cast.hpp>
#include <yave/ui/passkey.hpp>

namespace yave::ui {

  class view_context;
  class data_context;
  class window_event_dispatcher;

  namespace events {

    /// Window event base
    class window_event : public event
    {
    public:
      window_event(window* target, event_phase phase)
        : event(target, phase)
      {
      }

      void set_target(window* w, passkey<window_event_dispatcher>)
      {
        event::set_target(w);
      }

      void set_phase(event_phase p, passkey<window_event_dispatcher>)
      {
        event::set_phase(p);
      }

      void set_accepted(bool b, passkey<window_event_dispatcher>)
      {
        event::set_accepted(b);
      }
    };

    /// Window show event
    /// \note This event will not bubble.
    class show : public window_event
    {
    public:
      show(window* target)
        : window_event(target, event_phase::bubble)
      {
      }
    };

    /// Window hide event
    /// \note This event will not bubble.
    class hide : public window_event
    {
    public:
      hide(window* target)
        : window_event(target, event_phase::bubble)
      {
      }
    };

  } // namespace events

  namespace controllers {

    /// Window visibility
    class visibility : public controller
    {
    public:
      visibility()
        : controller(event_phase::bubble)
      {
      }

      bool event(ui::event& e, view_context& vctx) override
      {
        if (e.phase() == phase()) {
          /*  */ if (auto s = typeid_cast_if<events::show>(&e)) {
            assert(s->target() == window());
            s->accept();
            on_show(vctx);
          } else if (auto h = typeid_cast_if<events::hide>(&e)) {
            assert(h->target() == window());
            h->accept();
            on_hide(vctx);
          }
        }
        return false;
      }

      /// signaled before window become visible
      signal<view_context&> on_show;
      /// signaled before window become invisible
      signal<view_context&> on_hide;
    };

  } // namespace controllers

} // namespace yave::ui