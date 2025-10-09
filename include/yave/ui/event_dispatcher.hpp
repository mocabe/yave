//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/event.hpp>
#include <yave/ui/window.hpp>
#include <yave/ui/glfw_event_queue.hpp>

namespace yave::ui {

  class window_manager;
  class native_window;

  /// Window event dispatcher base
  class event_dispatcher
  {
    window_manager& m_wm;

    bool dispatch_capture_phase_impl(view& w, event& e);
    bool dispatch_bubble_phase_impl(view& w, event& e);

  public:
    event_dispatcher(window_manager& wm);

    /// Process event
    virtual void event(ui::event& e) = 0;

  protected:
    /// dispatch event over view's event controllers
    void dispatch_over_view(view& w, ui::event& e);
    /// dispatch capture phase
    bool dispatch_capture_phase(view& w, ui::event& e);
    /// dispatch bubble phase
    bool dispatch_bubble_phase(view& w, ui::event& e);
    /// find native view from glfw view handle
    auto find_view_from_handle(GLFWwindow* w) -> native_window*;
  };

} // namespace yave::ui