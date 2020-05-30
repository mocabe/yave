//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/mouse_events.hpp>
#include <yave/editor/view_context.hpp>

namespace yave::wm {

  // dispatch mouse event
  bool mouse_event_dispatcher::visit(window* w)
  {
    auto e = get_mouse_event();

    // hit detection
    if (m_view_ctx.window_manager().validate_pos(w, e->pos())) {
      if (event_dispatcher::visit(w)) {
        m_accepted = w;
        return true;
      }
    }
    return false;
  }
}