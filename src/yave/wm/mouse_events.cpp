//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/mouse_events.hpp>
#include <yave/editor/view_context.hpp>

namespace yave::wm {

  bool mouse_event_visitor::visit(window* w)
  {
    auto e = get_mouse_event();

    if (m_view_ctx.window_manager().hit_test(w, e->pos())) {

      if (!m_region)
        m_region = w;

      if (event_visitor::visit(w)) {
        m_accepted = w;
        return true;
      }
    }
    return false;
  }
}