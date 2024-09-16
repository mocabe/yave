//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/controller.hpp>

namespace yave::ui {

  controller::controller(event_phase phase)
    : m_phase {phase}
  {
  }

  void controller::set_window(ui::window& w, passkey<ui::window>)
  {
    m_window = &w;
  }

  void controller::clear_window(passkey<ui::window>)
  {
    m_window = nullptr;
  }

  void controller::set_phase(event_phase p)
  {
    m_phase = p;
  }

  auto controller::phase() -> event_phase
  {
    return m_phase;
  }

  bool controller::attached() const
  {
    return m_window != nullptr;
  }

  auto controller::window() -> ui::window&
  {
    return *m_window;
  }

  auto controller::window() const -> const ui::window&
  {
    return *m_window;
  }

} // namespace yave::ui