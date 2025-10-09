//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/glfw_event_queue.hpp>

namespace yave::ui {

  glfw_event_queue::glfw_event_queue(window_manager& wm)
    : m_wm {wm}
  {
  }

  auto glfw_event_queue::pop_window_event() -> std::optional<glfw_event>
  {
    if (!m_queue.empty()) {
      auto top = std::move(m_queue.front());
      m_queue.pop();
      return top;
    }
    return std::nullopt;
  }

  void glfw_event_queue::push_glfw_event(glfw_event event)
  {
    m_queue.push(std::move(event));
  }

  bool glfw_event_queue::has_pending_events() const
  {
    return !m_queue.empty();
  }

  void glfw_event_queue::dispatch_pending_events()
  {
    while (auto event = pop_window_event()) {
      signals.on_process_event(event);
    }
  }

} // namespace yave::ui