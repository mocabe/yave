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
  };

  /// Compiler thread operation interface for editor_data
  class compile_thread_interface
  {
    friend class compile_thread;

    /// compile thread ref
    compile_thread* m_thread_ptr = nullptr;
    /// parse errors
    error_list m_parse_errors = {};
    /// compile errors
    error_list m_compile_errors = {};
    /// result
    std::optional<executable> m_result;

  public:
    compile_thread_interface()                                = default;
    compile_thread_interface(const compile_thread_interface&) = delete;

    void init(compile_thread& th)
    {
      assert(!m_thread_ptr);
      m_thread_ptr = &th;
    }

    void deinit()
    {
      m_thread_ptr     = nullptr;
      m_parse_errors   = {};
      m_compile_errors = {};
      m_result         = std::nullopt;
    }

    void notify_recompile()
    {
      assert(m_thread_ptr);
      m_thread_ptr->notify_recompile();
    }

    auto& parse_errors() const
    {
      assert(m_thread_ptr);
      return m_parse_errors;
    }

    auto& compile_errors() const
    {
      assert(m_thread_ptr);
      return m_compile_errors;
    }

    auto get_result() const -> std::optional<executable>
    {
      if (m_result)
        return m_result->clone();
      else
        return std::nullopt;
    }
  };
} // namespace yave::editor