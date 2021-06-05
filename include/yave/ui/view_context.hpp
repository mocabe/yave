//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/command.hpp>
#include <yave/ui/data_context.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/key_events.hpp>
#include <yave/ui/mouse_events.hpp>

#include <memory>

namespace yave::ui {

  class view_context;
  class layout_context;
  class render_context;
  class glfw_context;
  class vulkan_context;

  /// View context command
  using view_command = command<view_context>;

  /// View context
  class view_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// Init view context
    view_context(data_context& dctx);
    /// Deinit view context
    ~view_context() noexcept;

  public:
    /// vulkan
    auto vulkan_ctx() -> vulkan_context&;
    auto vulkan_ctx() const -> const vulkan_context&;
    /// glfw
    auto glfw_ctx() -> glfw_context&;
    auto glfw_ctx() const -> const glfw_context&;
    /// data
    auto data_ctx() -> data_context&;
    auto data_ctx() const -> const data_context&;
    /// layout
    auto layout_ctx() -> ui::layout_context&;
    auto layout_ctx() const -> const ui::layout_context&;
    /// render
    auto render_ctx() -> ui::render_context&;
    auto render_ctx() const -> const ui::render_context&;

  private:
    void _post(view_command op) const;
    void _post_delay(view_command op) const;
    void _post_empty() const;

  public:
    /// Post view command
    template <class F>
    void post(F&& f)
    {
      _post(view_command(std::forward<F>(f)));
    }

    /// Post delayed view command
    /// \note This function is mainly designed to be called from main loop so we
    /// can guarantee execution of delayed commands are postponed until the
    /// beginning of next loop. You can still call this function from other
    /// threads but there's no guarantee about when posted commands are
    /// executed. Use task threads or timers for better control of delays.
    template <class F>
    void post_delay(F&& f)
    {
      _post_delay(view_command(std::forward<F>(f)));
    }

    /// Post empty command to wake view thread
    void wake()
    {
      _post_empty();
    }

  public:
    /// Start event loop.
    /// This function will not return until view thread exit.
    void run();
    /// Run event loop once.
    void poll();
    /// Exit view thread.
    void exit();

  public:
    /// get window manager
    auto window_manager() const -> const ui::window_manager&;
    auto window_manager() -> ui::window_manager&;
  };

} // namespace yave::ui