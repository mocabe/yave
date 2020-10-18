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
  class compile_thread_failure : std::runtime_error
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

  /// compile thread
  class compile_thread_data
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    compile_thread_data(data_context& dctx);
    ~compile_thread_data() noexcept;

  public:
    /// initialize compile thread.
    /// should be called after constructing related data.
    void init();

    /// deinit compile thread.
    /// should be called before destructing related data.
    void deinit();

  public:
    /// recompile graph
    void notify_recompile();

    /// get compile message
    auto messages() const -> const compiler::message_map&;

    /// get executable
    auto executable() -> std::optional<compiler::executable>&;

  public:
    /// clear compile results
    void clear_results();

    struct compile_results
    {
      compiler::message_map messages;
      std::optional<compiler::executable> exe;
    };

    /// set results
    void set_results(compile_results results);
  };

} // namespace yave::editor