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
    // success flags
    bool success = false;

    // timestamps
    std::chrono::steady_clock::time_point bgn_time =
      std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end_time =
      std::chrono::steady_clock::now();

    // result executable
    executable exe = {};

    // erros
    error_list compile_errors = {};
    error_list parse_errors   = {};
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

    /// Thread busy?
    bool is_compiling() const noexcept;

    /// Queue compilation
    void compile(
      const std::shared_ptr<const node_data_snapshot>& snapshot,
      const node_declaration_store& decls,
      const node_definition_store& defs);

    /// Get last executable
    auto get_last_result() const -> std::shared_ptr<compile_result>;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
} // namespace yave::app