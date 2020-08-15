//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/mouse_events.hpp>

#include <yave/lib/glfw/glfw_context.hpp>
#include <vector>

namespace yave::wm {

  class viewport_io;
  class viewport_window;

  /// viewport mouse event generator
  class mouse_event_emitter
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    mouse_event_emitter(viewport_io& io, window_manager& wm);
    /// dtor
    ~mouse_event_emitter() noexcept;

  public:
    /// dispatch events
    /// \param viewport parent viewport window used for root of event dispatch
    /// \note should call this after viewport_io::update()
    void dispatch_events(
      viewport_window* viewport,
      const editor::data_context& dctx,
      const editor::view_context& vctx);
  };
}