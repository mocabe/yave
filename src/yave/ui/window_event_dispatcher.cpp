//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/window_event_dispatcher.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/window_events.hpp>
#include <yave/ui/root.hpp>
#include <yave/ui/viewport.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/view_context.hpp>

#include <yave/config/fix.hpp>
#include <yave/support/overloaded.hpp>
#include <yave/support/log.hpp>
#include <queue>

#include <range/v3/to_container.hpp>

YAVE_DECL_LOCAL_LOGGER(ui::window_event_dispatcher)

namespace yave::ui {

  namespace {

    struct pos_event_data
    {
      GLFWwindow* win;
      u32 x, y;
    };

    struct size_event_data
    {
      GLFWwindow* win;
      u32 w, h;
    };

    struct close_event_data
    {
      GLFWwindow* win;
    };

    struct refresh_event_data
    {
      GLFWwindow* win;
    };

    struct fb_size_event_data
    {
      GLFWwindow* win;
      u32 w, h;
    };

    struct content_scale_event_data
    {
      GLFWwindow* win;
      f32 xs, ys;
    };

    struct focus_event_data
    {
      GLFWwindow* win;
      bool focused;
    };

    struct maximize_event_data
    {
      GLFWwindow* win;
      bool maximized;
    };

    struct minimize_event_data
    {
      GLFWwindow* win;
      bool minimized;
    };

    using event_data = std::variant<
      pos_event_data,
      size_event_data,
      close_event_data,
      refresh_event_data,
      fb_size_event_data,
      content_scale_event_data,
      focus_event_data,
      maximize_event_data,
      minimize_event_data>;

  } // namespace

  class window_event_dispatcher::impl
  {
    window_manager& m_wm;
    view_context& m_vctx;

    // event queue
    std::queue<event_data> m_queue;
    // focused window
    window* m_focused = nullptr;

  public:
    impl(window_manager& wm, view_context& vctx)
      : m_wm {wm}
      , m_vctx {vctx}
    {
    }

    bool has_pending_events() const
    {
      return !m_queue.empty();
    }

    // push window event data
    void push_window_event(event_data data)
    {
      m_queue.push(data);
    }

    // process window event data
    void dispatch_pending_events()
    {
      log_info("----- [[dispatch_pending_events]] -----");
      while (!m_queue.empty()) {
        std::visit([&](auto& e) { process(e); }, m_queue.front());
        m_queue.pop();
      }
    }

  private:
    // dispatch event to controllers of window
    void dispatch_over_window(window& w, event& e)
    {
      using namespace ranges;
      using namespace ranges::views;

      auto cs = std::vector<std::pair<tracker, controller&>>();
      cs.reserve(w.controllers().size());

      for (auto&& c : w.controllers()) {
        cs.emplace_back(c.get_tracker(), c);
      }

      for (auto&& [wp, c] : cs) {
        if (!wp.expired()) {
          if (c.event(e, m_vctx) || e.accepted())
          break;
      }
    }
    }

    // find native window
    auto find_native_from_handle(GLFWwindow* w) -> native_window*
    {
      for (auto&& vp : m_wm.root().viewports()) {
        auto native = &vp.native_window();
        if (native->handle() == w)
          return native;
      }
      return nullptr;
    }

  private:
    // common implementaiton of show/hide event
    template <class Event>
    void process_visibility_event(window& w)
    {
      auto notify_children = [&](auto&& self, window& w, Event& e) -> void {
        // not visible
        if (!w.is_visible())
          return;

        // reset and dispatch
        e.set_accepted(false, {});
        e.set_target(w, {});
        dispatch_over_window(w, e);

        for (auto&& c : w.children())
          self(c, e);
      };

      // dispatch on target window
      assert(w.is_registered());
      auto e = std::make_unique<Event>(w);
      dispatch_over_window(w, *e);

      for (auto&& c : w.children()) {
        auto f = fix_lambda(notify_children);
        f(c, *e);
      }
    }

  public:
    // send events::show
    void process_show_event(window& w)
    {
      if (!w.is_visible()) {
        process_visibility_event<events::show>(w);
        w.set_visible(true, {});
      }
    }

    // send events::hide
    void process_hide_event(window& w)
    {
      if (w.is_visible()) {
        process_visibility_event<events::hide>(w);
        w.set_visible(false, {});
      }
    }
    bool has_focused_window() const
    {
      return m_focused != nullptr;
    }

    auto focused_window() const -> window&
    {
      return *m_focused;
    }

    void set_focused_window(window& w)
    {
      assert(!has_focused_window());
      m_focused = &w;
    }

    void clear_focused_window()
    {
      m_focused = nullptr;
    }

  private:
    void process(const pos_event_data& data)
    {
      log_info("pos event: {},{}", data.x, data.y);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_pos(data.x, data.y, {});
      }
    }

    void process(const size_event_data& data)
    {
      log_info("size event: {},{}", data.w, data.h);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_size(data.w, data.h, {});
      }
    }

    void process(const close_event_data& data)
    {
      log_info("close event");
      if (auto native = find_native_from_handle(data.win)) {

        auto& viewport = native->viewport();

        // send close event
        auto event = std::make_unique<events::close>(viewport);
        dispatch_over_window(viewport, *event);

        if (event->accepted()) {
          // hide window before close
          native->hide();
          // destroy viewport and backing window
          m_wm.root().remove_viewport(viewport);
        }
      }
    }

    void process(const refresh_event_data& data)
    {
      log_info("refresh event");
      if (auto native = find_native_from_handle(data.win)) {
        native->viewport().invalidate();
      }
    }

    void process(const fb_size_event_data& data)
    {
      log_info("fb size event");
      if (auto native = find_native_from_handle(data.win)) {
        native->update_fb_size(data.w, data.h, {});
      }
    }

    void process(const content_scale_event_data& data)
    {
      log_info("content scale event");
      if (auto native = find_native_from_handle(data.win)) {
        native->update_content_scale(data.xs, data.ys, {});
      }
    }

    void process(const focus_event_data& data)
    {
      log_info("focus event: {}", data.focused);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_focus(data.focused, {});
      }
    }

    void process(const maximize_event_data& data)
    {
      log_info("maximize event: {}", data.maximized);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_maximize(data.maximized, {});
      }
    }

    void process(const minimize_event_data& data)
    {
      log_info("minimize event: {}", data.minimized);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_minimize(data.minimized, {});
      }
    }
  };

  window_event_dispatcher::window_event_dispatcher(
    window_manager& wm,
    view_context& vctx)
    : m_pimpl {std::make_unique<impl>(wm, vctx)}
  {
  }

  window_event_dispatcher::~window_event_dispatcher() noexcept = default;

  bool window_event_dispatcher::has_pending_events() const
  {
    return m_pimpl->has_pending_events();
  }

  void window_event_dispatcher::dispatch_pending_events()
  {
    m_pimpl->dispatch_pending_events();
  }

  void window_event_dispatcher::process_show_event(window& w)
  {
    m_pimpl->process_show_event(w);
  }

  void window_event_dispatcher::process_hide_event(window& w)
  {
    m_pimpl->process_hide_event(w);
  }

  void window_event_dispatcher::push_pos_event(GLFWwindow* win, u32 x, u32 y)
  {
    m_pimpl->push_window_event(pos_event_data {.win = win, .x = x, .y = y});
  }

  void window_event_dispatcher::push_size_event(GLFWwindow* win, u32 w, u32 h)
  {
    m_pimpl->push_window_event(size_event_data {.win = win, .w = w, .h = h});
  }

  void window_event_dispatcher::push_close_event(GLFWwindow* win)
  {
    m_pimpl->push_window_event(close_event_data {.win = win});
  }

  void window_event_dispatcher::push_refresh_event(GLFWwindow* win)
  {
    m_pimpl->push_window_event(refresh_event_data {.win = win});
  }

  void window_event_dispatcher::push_focus_event(GLFWwindow* win, bool focused)
  {
    m_pimpl->push_window_event(
      focus_event_data {.win = win, .focused = focused});
  }

  void window_event_dispatcher::push_fb_size_event(
    GLFWwindow* win,
    u32 w,
    u32 h)
  {
    m_pimpl->push_window_event(fb_size_event_data {.win = win, .w = w, .h = h});
  }

  void window_event_dispatcher::push_content_scale_event(
    GLFWwindow* win,
    f32 xs,
    f32 ys)
  {
    m_pimpl->push_window_event(
      content_scale_event_data {.win = win, .xs = xs, .ys = ys});
  }

  void window_event_dispatcher::push_maximize_event(
    GLFWwindow* win,
    bool maximized)
  {
    m_pimpl->push_window_event(
      maximize_event_data {.win = win, .maximized = maximized});
  }

  void window_event_dispatcher::push_minimize_event(
    GLFWwindow* win,
    bool minimized)
  {
    m_pimpl->push_window_event(
      minimize_event_data {.win = win, .minimized = minimized});
  }

} // namespace yave::ui