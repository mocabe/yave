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

  class window_event_dispatcher::impl
  {
    window_manager& m_wm;
    view_context& m_vctx;

    // event queue
    std::queue<glfw_event> m_queue;
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
    void push_window_event(glfw_event&& e)
    {
      m_queue.push(std::move(e));
    }

    auto pop_window_event() -> std::optional<glfw_event>
    {
      if (!m_queue.empty()) {
        auto top = std::move(m_queue.front());
        m_queue.pop();
        return top;
      }
      return std::nullopt;
    }

    // process window event data
    void dispatch_pending_events()
    {
      log_info("----- [[dispatch_pending_events]] -----");
      while (auto event = pop_window_event()) {
        std::visit([&](auto& e) { process(e); }, *event);
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
    void send_show_event(window& w)
    {
      if (!w.is_visible()) {
        process_visibility_event<events::show>(w);
        w.set_visible(true, {});
      }
    }

    // send events::hide
    void send_hide_event(window& w)
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
    void process(const glfw_pos_event& data)
    {
      log_info("pos event: {},{}", data.x, data.y);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_pos(data.x, data.y, {});
      }
    }

    void process(const glfw_size_event& data)
    {
      log_info("size event: {},{}", data.w, data.h);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_size(data.w, data.h, {});
      }
    }

    void process(const glfw_close_event& data)
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

    void process(const glfw_refresh_event& data)
    {
      log_info("refresh event");
      if (auto native = find_native_from_handle(data.win)) {
        native->viewport().invalidate();
      }
    }

    void process(const glfw_framebuffer_size_event& data)
    {
      log_info("fb size event");
      if (auto native = find_native_from_handle(data.win)) {
        native->update_framebuffer_size(data.w, data.h, {});
      }
    }

    void process(const glfw_content_scale_event& data)
    {
      log_info("content scale event");
      if (auto native = find_native_from_handle(data.win)) {
        native->update_content_scale(data.xs, data.ys, {});
      }
    }

    void process(const glfw_focus_event& data)
    {
      log_info("focus event: {}", data.focused);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_focus(data.focused, {});
      }
    }

    void process(const glfw_maximize_event& data)
    {
      log_info("maximize event: {}", data.maximized);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_maximize(data.maximized, {});
      }
    }

    void process(const glfw_minimize_event& data)
    {
      log_info("minimize event: {}", data.minimized);
      if (auto native = find_native_from_handle(data.win)) {
        native->update_minimize(data.minimized, {});
      }
    }

    void process(const glfw_cursor_enter_event& data)
    {
      log_info("cursor enter event (not implemented yet): {}", data.entered);
    }

    void process(const glfw_cursor_pos_event& data)
    {
      log_info(
        "cursor pos event (not implemented yet): {}, {}", data.xpos, data.ypos);
    }

    void process(const glfw_button_event& data)
    {
      log_info(
        "mouse button event (not implemented yet): b={}, a={}, m={}",
        (u32)data.button,
        (u32)data.action,
        (u32)data.mods);
    }

    void process(const glfw_key_event& data)
    {
      log_info(
        "key event (not implemented yet): k={}, a={}, m={}",
        (u32)data.key,
        (u32)data.action,
        (u32)data.mods);
    }

    void process(const glfw_char_event& data)
    {
      log_info("char event (not impelented yet): c={}", data.codepoint);
    }

    void process(const glfw_scroll_event& data)
    {
      log_info(
        "scroll event (not implemented yet): {}, {}",
        data.xoffset,
        data.yoffset);
    }

    void process(const glfw_path_drop_event& data)
    {
      log_info("path drop event (not implemented yet):");
      for (auto&& path : data.paths)
        log_info(" {}", (const char*)path.c_str());
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

  void window_event_dispatcher::push_glfw_event(glfw_event event)
  {
    m_pimpl->push_window_event(std::move(event));
  }

  void window_event_dispatcher::dispatch_pending_events()
  {
    m_pimpl->dispatch_pending_events();
  }

  void window_event_dispatcher::send_show_event(window& w)
  {
    m_pimpl->send_show_event(w);
  }

  void window_event_dispatcher::send_hide_event(window& w)
  {
    m_pimpl->send_hide_event(w);
  }

} // namespace yave::ui