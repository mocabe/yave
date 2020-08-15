//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
#include <yave/wm/event_dispatcher.hpp>
#include <yave/wm/window_traverser.hpp>
#include <yave/wm/root_window.hpp>

namespace yave::wm {

  /// Window mamanger class
  class window_manager
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// Initialize new window manager
    window_manager();
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

  public:
    /// calc screen pos of window
    /// \param win target window
    /// \return position of window in virtual screen coordinate
    [[nodiscard]] auto screen_pos(const window* win) const -> fvec2;

    /// check hit on window
    /// \param win target window
    /// \param pos position in virtual screen coordinate
    [[nodiscard]] bool intersects(const window* win, const fvec2& pos) const;

    /// no active viewport?
    [[nodiscard]] bool should_close() const;

    /// get current key focus
    [[nodiscard]] auto get_key_focus() const -> window*;

    /// set key focus
    void set_key_focus(window* win);

  public:
    /// process single frame
    void exec(editor::data_context& dctx, editor::view_context& vctx);
  };
} // namespace yave::wm