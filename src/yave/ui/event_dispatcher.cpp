//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/event_dispatcher.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/root.hpp>
#include <yave/ui/native_window.hpp>

#include <range/v3/to_container.hpp>

namespace yave::ui {

  event_dispatcher::event_dispatcher(window_manager& wm)
    : m_wm {wm}
  {
  }

  void event_dispatcher::dispatch_over_window(window& w, event& e)
  {
    using namespace ranges;
    using namespace ranges::views;

    auto cs = w.controllers()
              | ranges::views::transform([](auto&& c) { return weak_ref(c); })
              | ranges::to_vector;

    for (auto&& c : cs) {
      if (!c.expired()) {
        if (c.get()->event(e, m_wm.view_ctx()) || e.accepted())
          break;
      }
    }
  }

  bool event_dispatcher::dispatch_capture_phase_impl(window& w, event& e)
  {
    assert(e.phase() == event_phase::capture);

    if (!w.has_parent())
      return false;

    const auto t = w.get_tracker();

    if (dispatch_capture_phase_impl(w.parent(), e))
      return true;

    if (t.expired())
      return false;

    dispatch_over_window(w, e);
    return e.accepted();
  }

  bool event_dispatcher::dispatch_capture_phase(window& w, event& e)
  {
    return dispatch_capture_phase_impl(w, e);
  }

  bool event_dispatcher::dispatch_bubble_phase_impl(window& w, event& e)
  {
    assert(e.phase() == event_phase::bubble);

    if (!w.has_parent())
      return false;

    const auto t = w.get_tracker();

    dispatch_over_window(w, e);

    if (e.accepted())
      return true;

    if (t.expired())
      return false;

    return dispatch_bubble_phase_impl(w.parent(), e);
  }

  bool event_dispatcher::dispatch_bubble_phase(window& w, event& e)
  {
    return dispatch_bubble_phase_impl(w, e);
  }

  auto event_dispatcher::find_window_from_handle(GLFWwindow* w)
    -> native_window*
  {
    for (auto&& vp : m_wm.root().viewports()) {
      auto native = &vp.native_window();
      if (native->handle() == w)
        return native;
    }
    return nullptr;
  }

} // namespace yave::ui