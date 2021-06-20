//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/editor/data_context.hpp>

#include <memory>
#include <memory_resource>

namespace yave::editor {

  /// Get memory resource for data commands
  [[nodiscard]] auto get_data_command_memory_resource() noexcept
    -> std::pmr::memory_resource*;

  /// type of data commands
  enum class data_command_type
  {
    // single time commands
    single_time,
    // undoable commands
    undo_redo,
  };

  /// Data command is a kind of commands which are executed in data thread.
  /// Unlike notifiers, data operators need to support undo operation.
  /// This class also uses custom allocator.
  class data_command
  {
  public:
    /// Exec command
    virtual void exec(data_context& data_ctx) = 0;
    /// Undo command
    virtual void undo(data_context& data_ctx) = 0;
    /// Undoable command?
    virtual auto type() const -> data_command_type = 0;

    /// Dtor
    virtual ~data_command() noexcept = default;

    /// custom operator new
    void* operator new(size_t size)
    {
      std::pmr::polymorphic_allocator<std::byte> alloc(
        get_data_command_memory_resource());
      return alloc.allocate(size);
    }

    /// custom operator delete
    void operator delete(void* p, size_t size)
    {
      std::pmr::polymorphic_allocator<std::byte> alloc(
        get_data_command_memory_resource());
      alloc.deallocate((std::byte*)p, size);
    }
  };

  namespace detail {
    /// Helper class for single time data command
    template <class ExecFunc>
    struct lambda_data_command_st : public data_command, ExecFunc
    {
      lambda_data_command_st(ExecFunc&& exec)
        : ExecFunc(std::forward<ExecFunc>(exec))
      {
      }
      void exec(data_context& data_ctx) override
      {
        ExecFunc::operator()(data_ctx);
      }
      void undo(data_context&) override
      {
        // no undo
      }
      auto type() const -> data_command_type override
      {
        return data_command_type::single_time;
      }
    };

    /// Helper class for undoable data command
    template <class ExecFunc, class UndoFunc>
    struct lambda_data_command : public data_command, ExecFunc, UndoFunc
    {
      lambda_data_command(ExecFunc&& exec, UndoFunc&& undo)
        : ExecFunc(std::forward<ExecFunc>(exec))
        , UndoFunc(std::forward<UndoFunc>(undo))
      {
      }
      void exec(data_context& data_ctx) override
      {
        ExecFunc::operator()(data_ctx);
      }
      void undo(data_context& data_ctx) override
      {
        UndoFunc::operator()(data_ctx);
      }
      auto type() const -> data_command_type override
      {
        return data_command_type::undo_redo;
      }
    };
  } // namespace detail

  /// Create data command from lambda functions
  template <class ExecFunc>
  [[nodiscard]] auto make_data_command(ExecFunc&& exec)
  {
    return std::make_unique<detail::lambda_data_command_st<ExecFunc>>(
      std::forward<ExecFunc>(exec));
  }

  /// Create data command from lambda functions
  template <class ExecFunc, class UndoFunc>
  [[nodiscard]] auto make_data_command(ExecFunc&& exec, UndoFunc&& undo)
  {
    return std::make_unique<detail::lambda_data_command<ExecFunc, UndoFunc>>(
      std::forward<ExecFunc>(exec), std::forward<UndoFunc>(undo));
  }
} // namespace yave::editor