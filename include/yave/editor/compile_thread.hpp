//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/compiler/node_compiler.hpp>
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
    /// parse result
    node_parser_result m_parse_result = {};
    /// compile errors
    node_compiler_result m_compile_result = {};
    /// result
    std::optional<executable> m_result;

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
        auto lck         = m_thread_ptr->wait_task();
        m_thread_ptr     = nullptr;
        m_parse_result   = {};
        m_compile_result = {};
        m_result         = std::nullopt;
      }
    }

    void notify_recompile()
    {
      assert(m_thread_ptr);
      m_thread_ptr->notify_recompile();
    }

    auto& parse_result() const
    {
      assert(m_thread_ptr);
      return m_parse_result;
    }

    auto& compile_result() const
    {
      assert(m_thread_ptr);
      return m_compile_result;
    }
  };
} // namespace yave::editor