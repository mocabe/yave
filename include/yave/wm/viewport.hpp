//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/viewport_window.hpp>
#include <yave/wm/viewport_io.hpp>
#include <yave/wm/layout_window.hpp>
#include <yave/wm/mouse_event_emitter.hpp>
#include <yave/wm/key_event_emitter.hpp>
#include <yave/wm/window_manager.hpp>
#include <yave/wm/viewport_events.hpp>

#include <iostream>

namespace yave::wm {

  /// default viewport implementation.
  /// represents signle glfw window viewport.
  class viewport : public viewport_window
  {
    // window manager ref
    wm::window_manager& m_wm;
    /// glfw window
    glfw::glfw_window m_win;
    /// io interface
    wm::viewport_io m_io;
    /// mouse event generator
    wm::mouse_event_emitter m_mouse_event_emitter;
    /// key event generator
    wm::key_event_emitter m_key_event_emitter;

    void _emit_viewport_events(editor::data_context&, editor::view_context&);
    void _emit_io_events(editor::data_context&, editor::view_context&);
    void _emit_draw_events(editor::data_context&, editor::view_context&);

  public:
    viewport(wm::window_manager& wn, glfw::glfw_window&& win);

  public:
    /// set layout. current layout will be removed.
    /// \returns pointer to new layout window
    auto set_layout(std::unique_ptr<layout_window>&& layout) -> layout_window*;
    /// detach layout
    auto detach_layout() -> std::unique_ptr<layout_window>;
    /// remove layout
    void remove_layout();
    /// get current layout
    [[nodiscard]] auto layout() -> layout_window*;
    /// get current layout
    [[nodiscard]] auto layout() const -> const layout_window*;

  public:
    /// add new modal window
    auto add_modal(std::unique_ptr<window>&& modal) -> window*;
    /// detach modal window
    auto detach_modal(uid id) -> std::unique_ptr<window>;
    /// get list of modal window, from bottom to top
    auto modals() -> std::vector<window*>;
    /// get modal window from id
    auto modal(uid id) -> window*;

  public:
    bool should_close() const override;
    auto refresh_rate() const -> uint32_t override;
    void exec(editor::data_context&, editor::view_context&) override;

  public:
    void update(editor::data_context&, editor::view_context&) override;
    void draw(const editor::data_context&, const editor::view_context&)
      const override;

  public: // event handlers
    void on_resize(
      wm::events::resize& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
    void on_move(
      wm::events::move& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
    void on_draw(
      wm::events::draw& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
    void on_mouse_click(
      wm::events::mouse_click& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
    void on_mouse_double_click(
      wm::events::mouse_double_click& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
    void on_mouse_press(
      wm::events::mouse_press& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
    void on_mouse_release(
      wm::events::mouse_release& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
    void on_mouse_hover(
      wm::events::mouse_hover& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
    void on_key_press(
      wm::events::key_press& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
    void on_key_char(
      wm::events::key_char& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const override;
  };
}