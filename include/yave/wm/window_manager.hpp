//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
#include <yave/wm/event_dispatcher.hpp>

namespace yave::wm {

  /// Window mamanger class
  class window_manager
  {
  public:
    /// Initialize new window manager
    window_manager();
    /// destroy window
    ~window_manager() noexcept;

  public:
    /// add new window
    /// \param parent_id id of parent window
    /// \param index insert position of new window
    /// \param w new window to insert
    [[nodiscard]] auto add_window(
      uid parent_id,
      size_t index,
      std::unique_ptr<window>&& w) -> window*;

    /// remoev window
    /// \param window_id id of target window
    void remove_window(uid window_id);

  public:
    /// get root window
    [[nodiscard]] auto root() const -> window*;
    /// find window from ID
    [[nodiscard]] auto get_window(uid id) const -> window*;
    /// exists?
    [[nodiscard]] bool exists(uid id) const;

  public:
    /// process update on window tree
    void update(editor::data_context& dctx, editor::view_context& vctx);
    /// dispatch visitor to window tree
    void dispatch(window_visitor& visitor, window_traverser& traverser);

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
} // namespace yave::wm