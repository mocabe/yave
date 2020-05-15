//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/event_dispatcher.hpp>

namespace yave::wm {

  event_dispatcher::event_dispatcher(
    std::unique_ptr<event>&& e,
    editor::data_context& dctx,
    editor::view_context& vctx)
    : m_event {std::move(e)}
    , m_data_ctx {dctx}
    , m_view_ctx {vctx}
  {
  }

  bool event_dispatcher::visit(window* w)
  {
    w->emit(*m_event, m_data_ctx, m_view_ctx);
    return !m_event->accepted();
  }
} // namespace yave::wm