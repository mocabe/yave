//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/editor/view_command.hpp>
#include <yave/editor/data_context.hpp>

#include <yave/wm/window_manager.hpp>

#include <memory>

namespace yave::editor {

  class view_context;

  /// Access proxy of view_context
  class view_context_access
  {
    view_context& m_ctx;

  public:
    view_context_access(view_context& ctx)
      : m_ctx {ctx}
    {
    }

    /// access window manager
    auto window_manager() -> wm::window_manager&;
  };

  /// View context
  class view_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    view_context(data_context& dctx);
    /// dtor
    ~view_context() noexcept;

  public:
    /// push view command
    void cmd(std::unique_ptr<view_command>&& op) const;
    /// execute all view commands in queue
    void exec_all();

  public:
    /// process single frame
    void draw();

  public:
    /// wm
    auto window_manager() const -> const wm::window_manager&;
    /// wm
    auto window_manager() -> wm::window_manager&;
  };

  inline auto view_context_access::window_manager() -> wm::window_manager&
  {
    return m_ctx.window_manager();
  }

} // namespace yave::editor