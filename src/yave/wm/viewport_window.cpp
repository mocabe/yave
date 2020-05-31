//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/viewport_window.hpp>
#include <yave/wm/viewport_io.hpp>
#include <yave/wm/viewport_io_state.hpp>
#include <yave/wm/root_window.hpp>
#include <yave/editor/render_context.hpp>
#include <yave/editor/view_context.hpp>
#include <yave/wm/key_events.hpp>
#include <yave/wm/mouse_events.hpp>
#include <yave/support/log.hpp>

#include <random>

YAVE_DECL_G_LOGGER(viewport_window)

namespace yave::wm {

  viewport_window::viewport_window(
    wm::window_manager& wmng,
    glfw::glfw_window&& win)
    : window(
      std::u8string((const char8_t*)win.title().c_str()),
      win.pos(),
      win.size())
    , wm {wmng}
    , glfw_win {std::move(win)}
    , io {glfw_win}
    , io_state {wm}
    , graphics {glfw_win}
  {
    init_logger();
  }

  viewport_window::~viewport_window() noexcept = default;

  void viewport_window::render(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx)
  {
    auto& render_ctx = graphics.render_context();

    render_ctx.begin_frame(view_ctx);
    {
      render(data_ctx, view_ctx, render_ctx);
    }
    render_ctx.end_frame();
    render_ctx.render();
  }

  void viewport_window::events(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    auto traverser = std::make_unique<wm::dfs_traverser_reverse_pre>();

    // handle key events
    {
      auto input = io.query_key_input();

      // raw key events

      // key press
      for (auto&& key : input.press) {
        auto press = std::make_unique<wm::key_event_dispatcher>(
          std::make_unique<wm::events::key_press>(key), dctx, vctx);
        wm.dispatch(*press, *traverser);
      }

      // key release
      for (auto&& key : input.release) {
        auto release = std::make_unique<wm::key_event_dispatcher>(
          std::make_unique<wm::events::key_release>(key), dctx, vctx);
        wm.dispatch(*release, *traverser);
      }

      // key repeat
      for (auto&& key : input.repeat) {
        auto repeat = std::make_unique<wm::key_event_dispatcher>(
          std::make_unique<wm::events::key_repeat>(key), dctx, vctx);
        wm.dispatch(*repeat, *traverser);
      }
    }

    // handle mouse events
    {
      auto input = io.query_mouse_input();

      // raw mouse events

      // mouse press (1)
      for (auto&& button : input.press1) {
        auto dispatcher = std::make_unique<wm::mouse_press_dispatcher>(
          std::make_unique<wm::events::mouse_press>(button, input.cursor_pos),
          dctx,
          vctx);
        wm.dispatch(*dispatcher, *traverser);

        auto wndid = dispatcher->accepted() ? dispatcher->reciever()->id()
                                            : wm.root()->id();
        io_state.register_mouse_press1(wndid, button);
      }

      // mouse press (2+)
      for (auto&& button : input.press2) {
        auto dispatcher = std::make_unique<wm::mouse_press_dispatcher>(
          std::make_unique<wm::events::mouse_press>(button, input.cursor_pos),
          dctx,
          vctx);
        wm.dispatch(*dispatcher, *traverser);

        auto wndid = dispatcher->accepted() ? dispatcher->reciever()->id()
                                            : wm.root()->id();
        io_state.register_mouse_press2(wndid, button);
      }

      // mouse release
      for (auto&& button : input.release) {
        auto dispatcher = std::make_unique<wm::mouse_release_dispatcher>(
          std::make_unique<wm::events::mouse_release>(button, input.cursor_pos),
          dctx,
          vctx);
        wm.dispatch(*dispatcher, *traverser);

        auto wndid = dispatcher->accepted() ? dispatcher->reciever()->id()
                                            : wm.root()->id();
        io_state.register_mouse_release(wndid, button);
      }

      // mouse repeat
      for (auto&& button : input.repeat) {
        auto dispatcher = std::make_unique<wm::mouse_repeat_dispatcher>(
          std::make_unique<wm::events::mouse_repeat>(button, input.cursor_pos),
          dctx,
          vctx);
        wm.dispatch(*dispatcher, *traverser);
      }

      // mouse hover
      if (input.hovered) {
        auto dispatcher = std::make_unique<wm::mouse_hover_dispatcher>(
          std::make_unique<wm::events::mouse_hover>(input.cursor_pos),
          dctx,
          vctx);
        wm.dispatch(*dispatcher, *traverser);
      }

      // higher level mouse events

      for (auto&& button : io_state.query_mouse_click()) {
        auto dispatcher = std::make_unique<wm::mouse_click_dispatcher>(
          std::make_unique<wm::events::mouse_click>(button, input.cursor_pos),
          dctx,
          vctx);
        wm.dispatch(*dispatcher, *traverser);
      }

      for (auto&& button : io_state.query_mouse_double_click()) {
        auto dispatcher = std::make_unique<wm::mouse_double_click_dispatcher>(
          std::make_unique<wm::events::mouse_double_click>(
            button, input.cursor_pos),
          dctx,
          vctx);
        wm.dispatch(*dispatcher, *traverser);
      }
    }
  }

  bool viewport_window::should_close() const
  {
    return glfw_win.should_close();
  }

  auto viewport_window::fps() const -> uint32_t
  {
    return glfw_win.refresh_rate();
  }

  void viewport_window::render(
    editor::data_context& dc,
    editor::view_context& vc,
    editor::render_context& rc) const
  {
    auto drawer = rc.create_window_drawer(this);
    {
      // bg color
      rc.vulkan_window().set_clear_color(.5, .5, .2, 1);
      // draw children
      for (auto&& c : children())
        c->render(dc, vc, rc);
    }
  }

  /// handle viewport reisze and move
  /// \param pos position in virtual viewport
  /// \param size size of window (not framebuffer!)
  void viewport_window::resize(const fvec2& new_pos, const fvec2& new_size)
  {
    if (new_pos != pos()) {
      Info(g_logger, "window moved to {},{}", new_pos.x, new_pos.y);
      pos() = new_pos;
    }

    if (new_size != size()) {
      Info(g_logger, "window resized to {}, {}", new_size.x, new_size.y);
      size() = new_size;
      // propagete resize event to children
      for (auto&& c : children())
        as_mut_child(c)->resize({0, 0}, new_size);
    }
  }

  void viewport_window::update(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    // check window resize or move
    auto pos  = fvec2(glfw_win.pos());
    auto size = fvec2(glfw_win.size());
    resize(pos, size);

    // update
    for (auto&& c : children())
      as_mut_child(c)->update(dctx, vctx);
  }

  auto viewport_window::add_window(std::unique_ptr<window>&& win) -> window*
  {
    auto ret = win.get();
    add_any_window(children().end(), std::move(win));
    return ret;
  }

  void viewport_window::remove_window(uid id)
  {
    remove_any_window(id);
  }

  void viewport_window::on_mouse_click(
    wm::events::mouse_click& e,
    editor::data_context&,
    editor::view_context& vctx) const
  {
    auto p = e.pos() - vctx.window_manager().screen_pos(this);
    Info(g_logger, "mouse click {}, {}", p.x, p.y);
    e.accept();
  }

  void viewport_window::on_mouse_double_click(
    wm::events::mouse_double_click& e,
    editor::data_context&,
    editor::view_context& vctx) const
  {
    auto p = e.pos() - vctx.window_manager().screen_pos(this);
    Info(g_logger, "mouse double click {}, {}", p.x, p.y);
    e.accept();
  }

  void viewport_window::on_mouse_press(
    wm::events::mouse_press& e,
    editor::data_context&,
    editor::view_context& vctx) const
  {
    auto p = e.pos() - vctx.window_manager().screen_pos(this);
    Info(g_logger, "mouse press {}, {}", p.x, p.y);
    e.accept();
  }

  void viewport_window::on_mouse_release(
    wm::events::mouse_release& e,
    editor::data_context&,
    editor::view_context& vctx) const
  {
    auto p = e.pos() - vctx.window_manager().screen_pos(this);
    Info(g_logger, "mouse release {}, {}", p.x, p.y);
    e.accept();
  }

  void viewport_window::on_mouse_repeat(
    wm::events::mouse_repeat& e,
    editor::data_context&,
    editor::view_context& vctx) const
  {
    auto p = e.pos() - vctx.window_manager().screen_pos(this);
    Info(g_logger, "mouse repeat {}, {}", p.x, p.y);
    e.accept();
  }

  void viewport_window::on_mouse_hover(
    wm::events::mouse_hover& e,
    editor::data_context&,
    editor::view_context&) const
  {
    // auto p = e.pos() - screen_pos();
    // info(g_logger, "mouse hover {}, {}", p.x, p.y);
    e.accept();
  }

  void viewport_window::on_key_press(
    wm::events::key_press& e,
    editor::data_context&,
    editor::view_context&) const
  {
    e.accept();
  }

  void viewport_window::on_key_release(
    wm::events::key_release& e,
    editor::data_context&,
    editor::view_context&) const
  {
    e.accept();
  }

  void viewport_window::on_key_repeat(
    wm::events::key_repeat& e,
    editor::data_context&,
    editor::view_context&) const
  {
    e.accept();
  }

  void viewport_window::on_custom_event(
    wm::event& e,
    editor::data_context&,
    editor::view_context&) const
  {
    e.accept();
  }

} // namespace yave::wm