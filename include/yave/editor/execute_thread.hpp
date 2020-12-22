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
  class execute_thread_failure : public std::runtime_error
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

  /// Executor thread interface
  class execute_thread
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    execute_thread(data_context& dctx);
    execute_thread(execute_thread&&) noexcept;
    ~execute_thread() noexcept;

  public:
    /// initialize executor thread.
    /// should be called after constructing related data.
    void start();

    /// deinit executor thread.
    /// should be called before destructing related data.
    void stop();

    /// async execute with current arg time.
    void notify_execute();

    /// async execute with new arg time.
    void notify_execute(time time);
  };

  /// Execute thread data
  class execute_thread_data
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    execute_thread_data();
    execute_thread_data(execute_thread_data&&) noexcept;
    ~execute_thread_data() noexcept;

    /// get time argument to execute.
    auto last_arg_time() const -> yave::time;

    /// get result of last execution
    auto last_result_image() const -> const std::optional<image>&;

    /// get timestamp of last execution begin
    auto last_begin_time() const -> std::chrono::steady_clock::time_point;
    /// get timestamp of last execution end
    auto last_end_time() const -> std::chrono::steady_clock::time_point;

    /// get compute time of last execution
    auto last_compute_time() const -> std::chrono::milliseconds;

  private:
    friend class execute_thread;
    struct result_data
    {
      time arg_time;
      std::optional<yave::image> image;
      std::chrono::milliseconds compute_time;
      std::chrono::steady_clock::time_point begin_time;
      std::chrono::steady_clock::time_point end_time;
    };
    void set_result(result_data data);
  };
} // namespace yave::editor