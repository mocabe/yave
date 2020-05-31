//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
#include <yave/wm/event_dispatcher.hpp>
#include <yave/wm/viewport_window.hpp>
#include <yave/wm/root_window.hpp>

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/glfw/glfw_context.hpp>

namespace yave::wm {

  /// Window mamanger class
  class window_manager
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// Initialize new window manager
    window_manager(vulkan::vulkan_context& vkctx, glfw::glfw_context& glfwctx);
    /// destroy window
    ~window_manager() noexcept;

  public:
    /// get root window
    [[nodiscard]] auto root() -> wm::root_window*;
    /// get root window
    [[nodiscard]] auto root() const -> const wm::root_window*;

  public:
    /// exists?
    [[nodiscard]] bool exists(uid id) const;
    /// find window from ID
    [[nodiscard]] auto get_window(uid id) const -> const window*;
    /// find window from ID
    [[nodiscard]] auto get_window(uid id) -> window*;
    /// should close event loop?
    [[nodiscard]] bool should_close() const;

  public:
    /// get screen pos of window
    /// \requires exists(win->id())
    [[nodiscard]] auto screen_pos(const window* win) const -> fvec2;

    /// check if given position on screen lies on a window
    /// \param win target window
    /// \param pos virtual screen position
    /// \requires exists(win->id())
    [[nodiscard]] bool validate_pos(const window* win, const fvec2& pos) const;

  private:
    friend class editor::view_context;
    /// process update on window tree
    void update(editor::data_context& dctx, editor::view_context& vctx);
    /// render viewoprts
    void render(editor::data_context& dctx, editor::view_context& vctx);
    /// process viewport events
    void events(editor::data_context& dctx, editor::view_context& vctx);

  public:
    /// dispatch visitor to window tree
    void dispatch(window_visitor& visitor, window_traverser& traverser);

  public:
    /// add new viewport
    auto add_viewport(uint32_t width, uint32_t height, std::u8string name)
      -> viewport_window*;

    /// remove viewport
    void reomev_viewport(uid id);
  };
} // namespace yave::wm