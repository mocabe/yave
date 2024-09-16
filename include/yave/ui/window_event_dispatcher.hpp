//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/event.hpp>
#include <yave/ui/typedefs.hpp>
#include <yave/ui/key_events.hpp>
#include <yave/ui/mouse_events.hpp>
#include <yave/ui/glfw_events.hpp>

#include <memory>

namespace yave::ui {

  class view_context;
  class data_context;
  class window_manager;

  /// Dispatch window events
  class window_event_dispatcher
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    window_event_dispatcher(window_manager&, view_context&);
    ~window_event_dispatcher() noexcept;

  public:
    /// Push event to queue
    void push_glfw_event(glfw_event event);
    /// Check if there's any event(s) pending to dispatch
    bool has_pending_events() const;
    /// Dispatch
    void dispatch_pending_events();

  public:
    /// Send show event
    void send_show_event(window& w);
    /// Send hide event
    void send_hide_event(window& w);
    /// Send focus event
    void send_focus_event(window& w);
    /// Send blur event
    void send_blur_event(window& w);
  };

} // namespace yave::ui