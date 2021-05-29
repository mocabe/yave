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
      window_event(window& target, event_phase phase);
    };

    /// Window show event
    /// \note This event will not bubble.
    class show : public window_event
    {
      using event::set_accepted;
      using event::accept;
      using event::ignore;
      using event::accepted;

    public:
      show(window& target);

      void set_target(window& w, passkey<window_event_dispatcher>);
      void set_phase(event_phase p, passkey<window_event_dispatcher>);
      void set_accepted(bool b, passkey<window_event_dispatcher>);
    };

    /// Window hide event
    /// \note This event will not bubble.
    class hide : public window_event
    {
      using event::set_accepted;
      using event::accept;
      using event::ignore;
      using event::accepted;

    public:
      hide(window& target);

      void set_target(window& w, passkey<window_event_dispatcher>);
      void set_phase(event_phase p, passkey<window_event_dispatcher>);
      void set_accepted(bool b, passkey<window_event_dispatcher>);
    };

    /// Window close event
    /// \note This event will not buuble.
    class close : public window_event
    {
    public:
      close(window& target);
    };

  } // namespace events

} // namespace yave::ui