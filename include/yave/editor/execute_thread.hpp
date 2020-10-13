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

  /// execute thread
  class execute_thread
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    execute_thread(data_context& dctx);
    /// dtor
    ~execute_thread() noexcept;

  public:
    /// notify execution
    void notify_execute();
    /// wait current task and block next one
    auto wait_task() -> std::unique_lock<std::mutex>;
  };

  /// Execute thread interface
  class execute_thread_interface
  {
    friend class execute_thread;

    /// execute thread ref
    execute_thread* m_thread_ptr = nullptr;
    /// time to execute
    yave::time m_time;
    /// execution result
    std::optional<image> m_result = std::nullopt;
    /// execution time
    std::chrono::nanoseconds m_exec_time = {};
    /// timestamp
    std::chrono::steady_clock::time_point m_timestamp =
      std::chrono::steady_clock::now();

  public:
    execute_thread_interface()                                = default;
    execute_thread_interface(const execute_thread_interface&) = delete;
    execute_thread_interface(execute_thread_interface&&)      = default;

    void init(execute_thread& th)
    {
      m_thread_ptr = &th;
    }

    bool initialized() const
    {
      return m_thread_ptr;
    }

    void deinit()
    {
      if (m_thread_ptr) {
        auto lck     = m_thread_ptr->wait_task();
        m_thread_ptr = nullptr;
        m_result     = std::nullopt;
      }
    }

    void notify_execute()
    {
      assert(m_thread_ptr);
      m_thread_ptr->notify_execute();
    }

    auto& time() const
    {
      return m_time;
    }

    void set_time(const yave::time& newtime)
    {
      m_time = newtime;
    }

    auto& exec_time() const
    {
      return m_exec_time;
    }

    auto& timestamp() const
    {
      return m_timestamp;
    }

    auto& get_result() const
    {
      return m_result;
    }
  };
} // namespace yave::editor