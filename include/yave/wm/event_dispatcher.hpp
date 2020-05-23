//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/window.hpp>
#include <yave/wm/window_visitor.hpp>

namespace yave::wm {

  /// Event dispatcher
  struct event_dispatcher : window_visitor
  {
    /// Ctor
    event_dispatcher(
      std::unique_ptr<event>&& e,
      editor::data_context& dctx,
      editor::view_context& vctx);

    /// Dtor
    ~event_dispatcher() noexcept = default;

    /// Dispatch event to window
    bool visit(window* w) override;

  protected:
    std::unique_ptr<event> m_event;
    editor::data_context& m_data_ctx;
    editor::view_context& m_view_ctx;
  };
} // namespace yave::wm