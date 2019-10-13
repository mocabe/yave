//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/compiler/executable.hpp>
#include <yave/node/compiler/errors.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/core/bind_info_manager.hpp>
#include <yave/node/core/socket_instance_manager.hpp>

namespace yave {

  class node_compiler
  {
  public:
    /// Ctor
    node_compiler();

    /// Compile parsed graph
    [[nodiscard]] auto compile(
      parsed_node_graph&& input,
      const bind_info_manager& bim) -> std::optional<executable>;

    /// Get last errors
    [[nodiscard]] auto get_errors() const -> error_list;

  private:
    /// Optimize parsed graph
    [[nodiscard]] auto _optimize(parsed_node_graph&& parsed_graph)
      -> std::optional<parsed_node_graph>;

    /// Resolve overloadings and check type
    [[nodiscard]] auto _type(
      const parsed_node_graph& graph,
      const bind_info_manager& bim) -> std::optional<socket_instance_manager>;

    /// Generate apply graph
    [[nodiscard]] auto _generate(
      const parsed_node_graph& graph,
      const socket_instance_manager& sim) -> std::optional<executable>;

    /// Verbose type check
    [[nodiscard]] auto _verbose_check(
      const parsed_node_graph& pasred_graph,
      const socket_instance_manager& sim,
      const executable& executable) -> bool;

  private:
    [[nodiscard]] auto _lock() const -> std::unique_lock<std::mutex>;

  private:
    mutable std::mutex m_mtx;

  private:
    error_list m_errors;
  };

} // namespace yave