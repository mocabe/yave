//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/glfw_events.hpp>
#include <yave/ui/signal.hpp>

#include <queue>

namespace yave::ui {

  class window_manager;

  /// GLFW event queue
  class glfw_event_queue
  {
    window_manager& m_wm;
    std::queue<glfw_event> m_queue;

    auto pop_window_event() -> std::optional<glfw_event>;

  public:
    struct _signals
    {
      /// called when queue processes glfw event
      signal<const glfw_event&> on_process_event;
    } signals;

  public:
    glfw_event_queue(window_manager& wm);

    /// Push event to queue
    void push_glfw_event(glfw_event event);
    /// Check if there's any event(s) pending to dispatch
    bool has_pending_events() const;
    /// Dispatch
    void dispatch_pending_events();
  };
}