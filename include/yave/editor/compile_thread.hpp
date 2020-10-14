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

  /// compile thread
  class compile_thread
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    compile_thread(data_context& dctx);
    /// dtor
    ~compile_thread() noexcept;

  public:
    /// recompile graph
    void notify_recompile();
    /// wait current task finish and block next task
    auto wait_task() -> std::unique_lock<std::mutex>;
  };

  /// Compiler thread operation interface for editor_data
  class compile_thread_interface
  {
    friend class compile_thread;

    /// compile thread ref
    compile_thread* m_thread_ptr = nullptr;
    /// compile result
    compiler::message_map m_messages;
    /// result
    std::optional<compiler::executable> m_exe;

  public:
    compile_thread_interface()                                    = default;
    compile_thread_interface(const compile_thread_interface&)     = delete;
    compile_thread_interface(compile_thread_interface&&) noexcept = default;

    void init(compile_thread& th)
    {
      assert(!m_thread_ptr);
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
        m_messages   = {};
        m_exe     = std::nullopt;
      }
    }

    void clear_results()
    {
      m_messages = {};
      m_exe      = std::nullopt;
    }

    void notify_recompile()
    {
      assert(m_thread_ptr);
      m_thread_ptr->notify_recompile();
    }

    auto& messages() const
    {
      assert(m_thread_ptr);
      return m_messages;
    }

    auto& executable() const
    {
      assert(m_thread_ptr);
      return m_exe;
    }
  };
} // namespace yave::editor