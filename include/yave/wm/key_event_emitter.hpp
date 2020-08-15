//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/key_events.hpp>

namespace yave::wm {

  class viewport_io;
  class viewport_window;

  class key_event_emitter
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    key_event_emitter(viewport_io& io, window_manager& wm);
    /// dtor
    ~key_event_emitter() noexcept;

  public:
    /// dispatch key events
    void dispatch_events(
      viewport_window* viewport,
      const editor::data_context& dctx,
      const editor::view_context& vctx);
  };
}