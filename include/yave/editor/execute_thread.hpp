//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/editor/data_context.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/lib/time/time.hpp>

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
  };

  /// Execute thread interface
  class execute_thread_interface
  {
    friend class execute_thread;

    /// execute thread ref
    execute_thread* m_thread_ptr = nullptr;
    /// time to execute
    time m_time;
    /// execution result
    object_ptr<const FrameBuffer> m_result;

  public:
    execute_thread_interface()                                = default;
    execute_thread_interface(const execute_thread_interface&) = delete;

    void init(execute_thread& th)
    {
      m_thread_ptr = &th;
    }

    void deinit()
    {
      m_thread_ptr = nullptr;
      m_result     = nullptr;
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

    auto& get_result() const
    {
      return *m_result;
    }
  };
} // namespace yave::editor