//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/compiler/node_compiler.hpp>
#include <yave/app/node_data_thread.hpp>

namespace yave::app {

  struct compile_result
  {
    bool success;
    std::chrono::steady_clock::time_point bgn_time;
    std::chrono::steady_clock::time_point end_time;
    executable exe;
    error_list compile_errors;
    error_list parse_errors;
  };

  /// Node compiler thread
  class node_compiler_thread
  {
  public:
    /// Ctor
    node_compiler_thread();
    /// Dtor
    ~node_compiler_thread() noexcept;

    /// Start
    void start();

    /// Stop
    void stop();

    /// Thread running?
    bool is_running() const noexcept;

    /// Queue compilation
    void compile(
      const std::shared_ptr<const node_data_snapshot>& snapshot,
      const node_definition_store& defs);

    /// Get last executable
    auto get_last_result() const -> std::shared_ptr<compile_result>;

  private:
    std::thread m_thread;
    std::atomic<int> m_terminate_flag;
    std::atomic<int> m_compile_flag;
    std::mutex m_mtx;
    std::condition_variable m_cond;
    struct _queue_data;
    std::queue<_queue_data> m_queue;
    std::shared_ptr<compile_result> m_result;
    node_parser m_parser;
    node_compiler m_compiler;
  };
} // namespace yave::app