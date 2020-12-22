//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/compiler/message.hpp>
#include <yave/compiler/executable.hpp>

#include <yave/editor/data_context.hpp>

#include <optional>

namespace yave::editor {

  /// unexpected thread failure
  class compile_thread_failure : public std::runtime_error
  {
    std::exception_ptr m_exception;

  public:
    compile_thread_failure(std::exception_ptr exception)
      : std::runtime_error("compile thread failed by uncaught exception")
      , m_exception {exception}
    {
    }

    /// get exception thrown in execute thread to it
    [[nodiscard]] auto exception() const
    {
      return m_exception;
    }
  };

  /// compile thread interface
  class compile_thread
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    compile_thread(data_context& dctx);
    compile_thread(compile_thread&&) noexcept;
    ~compile_thread() noexcept;

  public:
    /// initialize compile thread.
    /// should be called after constructing related data.
    void start();

    /// deinit compile thread.
    /// should be called before destructing related data.
    void stop();

    /// recompile graph
    void notify_compile();
  };

  /// compile thread data
  class compile_thread_data
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    compile_thread_data();
    compile_thread_data(compile_thread_data&&) noexcept;
    ~compile_thread_data() noexcept;

  public:
    /// get compile message
    auto last_message() const -> const compiler::message_map&;

    /// get executable
    auto last_executable() const -> const std::optional<compiler::executable>&;

  private:
    friend class compile_thread;

    struct compile_results
    {
      compiler::message_map last_msg;
      std::optional<compiler::executable> last_exe;
    };
    void set_results(compile_results results);
    void clear_results();
  };

} // namespace yave::editor