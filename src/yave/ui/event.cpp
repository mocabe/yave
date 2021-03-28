//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/event.hpp>

namespace yave::ui {

  event::event(const window* target, event_phase phase)
    : m_target {target}
    , m_phase {phase}
  {
  }

  event::~event() noexcept = default;

  auto event::target() const -> const window*
  {
    return m_target;
  }

  auto event::phase() const -> event_phase
  {
    return m_phase;
  }

  bool event::accepted() const
  {
    return m_accepted;
  }

  void event::set_target(window* w)
  {
    m_accepted = w;
  }

  void event::set_phase(event_phase p)
  {
    m_phase = p;
  }

  void event::set_accepted(bool b)
  {
    m_accepted = b;
  }

  void event::accept()
  {
    set_accepted(true);
  }
}