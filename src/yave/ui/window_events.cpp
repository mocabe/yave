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

  show::show(window& target)
    : window_event(target, event_phase::bubble)
  {
  }

  void show::set_target(window& w, passkey<window_event_dispatcher>)
  {
    event::set_target(w);
  }

  void show::set_phase(event_phase p, passkey<window_event_dispatcher>)
  {
    event::set_phase(p);
  }

  void show::set_accepted(bool b, passkey<window_event_dispatcher>)
  {
    event::set_accepted(b);
  }

  hide::hide(window& target)
    : window_event(target, event_phase::bubble)
  {
  }

  void hide::set_target(window& w, passkey<window_event_dispatcher>)
  {
    event::set_target(w);
  }

  void hide::set_phase(event_phase p, passkey<window_event_dispatcher>)
  {
    event::set_phase(p);
  }

  void hide::set_accepted(bool b, passkey<window_event_dispatcher>)
  {
    event::set_accepted(b);
  }

  close::close(window& target)
    : window_event(target, event_phase::bubble)
  {
  }
}