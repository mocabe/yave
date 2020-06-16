//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
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
    /// update windows
    void update(editor::data_context& dctx, editor::view_context& vctx);
    /// draw windows
    void draw(editor::data_context& dctx, editor::view_context& vctx);
  };
} // namespace yave::wm