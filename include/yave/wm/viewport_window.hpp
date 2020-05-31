//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
#include <yave/wm/viewport_io.hpp>
#include <yave/wm/viewport_io_state.hpp>
#include <yave/wm/viewport_graphics.hpp>

namespace yave::wm {

  // fwd
  class window_manager;

  /// Toplevel viewport window.
  /// This window will be added under root window at initialization of
  /// application.
  /// Every frame, this window will call reisze() to all child windows if
  /// needed.
  class viewport_window final : public wm::window
  {
    // window manager ref
    wm::window_manager& wm;
    /// vulkan
    vulkan::vulkan_context& vk;
    /// glfw window
    glfw::glfw_window glfw_win;
    /// io interface
    wm::viewport_io io;
    /// io state
    wm::viewport_io_state io_state;
    /// rendering context
    wm::viewport_graphics graphics;

  public:
    /// Construct viewport from GLFW window.
    viewport_window(
      wm::window_manager& wm,
      vulkan::vulkan_context& vk_ctx,
      glfw::glfw_window&& glfw_win);

    /// dtor
    ~viewport_window() noexcept;

  public:
    /// render viewport
    void render(editor::data_context& data_ctx, editor::view_context& view_ctx);
    /// dispatch viewport events
    void events(editor::data_context& data_ctx, editor::view_context& view_ctx);

  public:
    /// close
    bool should_close() const;
    /// fps
    auto fps() const -> uint32_t;

  public:
    /// clear framebuffer
    void render(
      editor::data_context& dc,
      editor::view_context& vc,
      editor::render_context& rc) const override;

    /// Handle viewport reisze and move.
    /// This function will be called from update().
    /// \param pos position in virtual viewport
    /// \param size size of window (not framebuffer!)
    void resize(const fvec2& pos, const fvec2& size) override;

    /// Handle viewport window updates, call resize().
    void update(editor::data_context& dc, editor::view_context& vc) override;

  public:
    /// Add child window
    auto add_window(std::unique_ptr<window>&& win) -> window*;
    /// Remove child window
    void remove_window(uid id);

  public:
    void on_mouse_click(
      wm::events::mouse_click& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;

    void on_mouse_double_click(
      wm::events::mouse_double_click& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;

    void on_mouse_press(
      wm::events::mouse_press& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;

    void on_mouse_release(
      wm::events::mouse_release& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;

    void on_mouse_repeat(
      wm::events::mouse_repeat& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;

    void on_mouse_hover(
      wm::events::mouse_hover& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;

    void on_key_press(
      wm::events::key_press& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;

    void on_key_release(
      wm::events::key_release& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;

    void on_key_repeat(
      wm::events::key_repeat& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;

    void on_custom_event(
      wm::event& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) const override;
  };

} // namespace yave::wm