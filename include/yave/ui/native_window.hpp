//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/passkey.hpp>
#include <yave/ui/size.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <memory>
#include <span>

namespace yave::ui {

  class view_context;
  class window_event_dispatcher;
  class viewport;
  class vulkan_surface;

  /// Native window class
  class native_window
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    native_window(
      view_context& vctx,
      std::u8string name,
      size size,
      passkey<viewport>&&);

    ~native_window() noexcept;

  public:
    // for viewport
    void set_viewport(viewport& vp, passkey<viewport>);

  public:
    /// native handle
    auto handle() const -> GLFWwindow*;
    /// window name
    auto name() const -> const std::u8string&;
    /// window size
    auto size() const -> ui::size;
    /// winow pos
    auto pos() const -> ui::vec;
    /// framebuffer size
    auto framebuffer_size() const -> ui::size;
    /// content scaling
    auto content_scale() const -> ui::vec;
    /// framebuffer scale
    auto framebuffer_scale() const -> ui::vec;
    /// focus
    bool focused() const;
    /// maximize
    bool maximized() const;
    /// minimize
    bool minimized() const;

  public:
    /// change name of window
    void set_name(std::u8string name);
    /// resize window
    void set_size(ui::size size);
    /// reposition winow
    void set_pos(ui::vec pos);

  public:
    // update pos/size on event
    void update_pos(u32 x, u32 y, passkey<window_event_dispatcher>);
    void update_size(u32 w, u32 h, passkey<window_event_dispatcher>);
    void update_framebuffer_size(u32 w, u32 h, passkey<window_event_dispatcher>);
    void update_content_scale(f32 xs, f32 ys, passkey<window_event_dispatcher>);
    void update_focus(bool focused, passkey<window_event_dispatcher>);
    void update_maximize(bool maximized, passkey<window_event_dispatcher>);
    void update_minimize(bool minimized, passkey<window_event_dispatcher>);

  public:
    /// get viewport
    auto viewport() -> ui::viewport&;
    auto viewport() const -> const ui::viewport&;

  public:
    /// show window
    void show();
    /// hide window
    void hide();
  };

} // namespace yave::ui