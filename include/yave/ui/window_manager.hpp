//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/window.hpp>
#include <yave/ui/signal.hpp>
#include <yave/ui/glfw_events.hpp>

#include <memory>

namespace yave::ui {

  class root;
  class viewport;
  class view_context;
  class data_context;
  class layout_context;

  /// Window mamanger class
  class window_manager
  {
  public:
    struct _signals
    {
      /// will be called just after registered  window
      signal<window&> on_register;
      /// will be called just before unregister window
      signal<window&> on_unregister;
      /// will be called just after invalidated window
      signal<window&> on_invalidate;
      /// will be called when all windows are closed
      signal<> on_last_window_close;
    };
    /// mostly for internal use
    _signals signals;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// Initialize new window manager
    window_manager(view_context& vctx);
    /// destroy window
    ~window_manager() noexcept;

  public:
    /// get view ref
    auto view_ctx() -> view_context&;

  public:
    /// push event
    void push_glfw_event(glfw_event e, passkey<glfw_context>);
    /// number of pending events
    bool has_pending_events(passkey<view_context>) const;
    /// process all pending events
    void dispatch_pending_events(passkey<view_context>);
    /// clear invalidated flags
    void clear_invalidated(passkey<view_context>);

  public:
    /// get root window
    auto root() -> ui::root&;
    /// get root window
    auto root() const -> const ui::root&;

  public:
    /// exists?
    bool exists(wid id) const;
    /// find window from ID
    auto find(wid id) const -> const window*;
    /// find window from ID
    auto find(wid id) -> window*;

  public:
    /// child?
    /// \note returns false on !exists(c) || !exists(p)
    bool is_child(const window& c, const window& p) const;
    /// parent?
    /// \note returns false on !exists(p) || !exists(c)
    bool is_parent(const window& p, const window& c) const;

  public:
    // for window
    void invalidate_window(window&, passkey<window>);
    void register_window(window&, passkey<window>);
    void unregister_window(window&, passkey<window>);
    void show_window(window&, passkey<window>);
    void hide_window(window&, passkey<window>);
  };

} // namespace yave::ui
