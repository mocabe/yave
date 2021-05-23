//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/window_events.hpp>

namespace yave::ui::events {

  window_event::window_event(window& target, event_phase phase)
    : event(target, phase)
  {
  }

  void window_event::set_target(window& w, passkey<window_event_dispatcher>)
  {
    event::set_target(w);
  }

  void window_event::set_phase(event_phase p, passkey<window_event_dispatcher>)
  {
    event::set_phase(p);
  }

  void window_event::set_accepted(bool b, passkey<window_event_dispatcher>)
  {
    event::set_accepted(b);
  }

  show::show(window& target)
    : window_event(target, event_phase::bubble)
  {
  }

  hide::hide(window& target)
    : window_event(target, event_phase::bubble)
  {
  }

  close::close(window& target)
    : window_event(target, event_phase::bubble)
  {
  }
}