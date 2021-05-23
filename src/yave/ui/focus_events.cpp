//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/focus_events.hpp>

namespace yave::ui::events
{

  focus_event::focus_event(
    const window& target,
    event_phase phase,
    focus_reason reason)
    : event(target, phase)
    , m_reason {reason}
  {
  }

  auto focus_event::reason() const -> focus_reason
  {
    return m_reason;
  }

  focusing::focusing(
    const window& target,
    event_phase phase,
    focus_reason reason)
    : focus_event(target, phase, reason)
  {
  }

  blurring::blurring(
    const window& target,
    event_phase phase,
    focus_reason reason)
    : focus_event(target, phase, reason)
  {
  }

  focus::focus(const window& target, focus_reason reason)
    : focus_event(target, event_phase::bubble, reason)
  {
  }

  blur::blur(const window& target, focus_reason reason)
    : focus_event(target, event_phase::bubble, reason)
  {
  }
} // namespace yave::ui::events