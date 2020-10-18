//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/editor/data_context.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/lib/time/time.hpp>
#include <yave/lib/image/image.hpp>

#include <optional>

namespace yave::editor {

  /// unexpected thread failure
  class execute_thread_failure : std::runtime_error
  {
    std::exception_ptr m_exception;

  public:
    execute_thread_failure(std::exception_ptr exception)
      : std::runtime_error("execution thread failed by uncaught exception")
      , m_exception {exception}
    {
    }

    /// get exception thrown in execute thread to it
    [[nodiscard]] auto exception() const
    {
      return m_exception;
    }
  };

  /// Execute thread interface
  class execute_thread_data
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    execute_thread_data(data_context& dctx);
    ~execute_thread_data() noexcept;
    execute_thread_data(const execute_thread_data&) = delete;

    /// initialize executor thread.
    /// should be called after constructing related data.
    void init();

    /// deinit executor thread.
    /// should be called before destructing related data.
    void deinit();

    /// send notification to thread.
    void notify_execute();

    /// get time argument to execute.
    auto arg_time() const -> yave::time;
    /// set time argument to execute.
    void set_arg_time(yave::time t);

    /// get duration required to run execution last time
    auto exec_duration() const -> std::chrono::milliseconds;

    /// get timestamp of execution finished last time
    auto exec_timestamp() const -> std::chrono::steady_clock::time_point;

    /// access to result of execution
    auto exec_result() -> std::optional<yave::image>&;

  public:
    struct exec_results
    {
      std::optional<yave::image> img;
      std::chrono::milliseconds duration;
      std::chrono::steady_clock::time_point timestamp;
    };

    /// should be called from execute thread
    void set_exec_results(exec_results results);
  };
} // namespace yave::editor