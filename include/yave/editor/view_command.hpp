//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <memory>
#include <memory_resource>

namespace yave::editor {

  // fwd
  class view_context;

  /// Get memory resource for view commands
  [[nodiscard]] auto get_view_command_memory_resource() noexcept
    -> std::pmr::memory_resource*;

  /// View command repsents modification on view model.
  class view_command
  {
  public:
    /// command
    virtual void exec(view_context& view_ctx) = 0;

    /// dtor
    virtual ~view_command() noexcept = default;

    /// custom operator new
    void* operator new(size_t size)
    {
      std::pmr::polymorphic_allocator<std::byte> alloc(
        get_view_command_memory_resource());
      return alloc.allocate(size);
    }

    /// custom operator delete
    void operator delete(void* p, size_t size)
    {
      std::pmr::polymorphic_allocator<std::byte> alloc(
        get_view_command_memory_resource());
      alloc.deallocate((std::byte*)p, size);
    }
  };

  namespace detail {

    template <class ExecFunc>
    struct lambda_view_command : public view_command, ExecFunc
    {
      lambda_view_command(ExecFunc&& exec)
        : ExecFunc {std::forward<ExecFunc>(exec)}
      {
      }

      void exec(view_context& view_ctx) override
      {
        ExecFunc::operator()(view_ctx);
      }
    };

  } // namespace detail

  /// Create view command from lambda functions
  template <class ExecFunc>
  [[nodiscard]] auto make_view_command(ExecFunc&& exec)
  {
    return std::make_unique<detail::lambda_view_command<ExecFunc>>(
      std::forward<ExecFunc>(exec));
  }

  /// Create view command for specific window
  template <class ExecFunc, class Window>
  [[nodiscard]] auto make_window_view_command(const Window& w, ExecFunc&& exec)
  {
    return make_view_command(
      [exec = std::forward<ExecFunc>(exec), id = w.id()](auto& ctx) {
        if (auto w = ctx.window_manager().get_window(id))
          exec(*w->template as<Window>());
      });
  }

} // namespace yave::editor