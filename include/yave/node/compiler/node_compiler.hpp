//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/compiler/executable.hpp>
#include <yave/node/compiler/errors.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/support/bind_info_manager.hpp>
#include <yave/node/support/socket_instance_manager.hpp>

namespace yave {

  class node_compiler
  {
  public:
    node_compiler();

    /// Compile parsed graph
    auto compile(parsed_node_graph&& input, const bind_info_manager& bim)
      -> std::optional<executable>;

    /// Get last errors
    auto get_errors() const -> error_list;

  private:
    /// Optimize parsed graph
    auto _optimize(parsed_node_graph&& parsed_graph)
      -> std::optional<parsed_node_graph>;
    /// Resolve overloadings and check type
    auto _type(const parsed_node_graph& graph, const bind_info_manager& bim)
      -> std::optional<socket_instance_manager>;
    /// Generate apply graph
    auto _generate(
      const parsed_node_graph& graph,
      const socket_instance_manager& sim) -> std::optional<executable>;
    /// Verbose type check
    auto _verbose_check(
      const parsed_node_graph& pasred_graph,
      const socket_instance_manager& sim,
      const executable& executable) -> bool;

  private:
    auto _lock() const -> std::unique_lock<std::mutex>;

  private:
    mutable std::mutex m_mtx;

  private:
    error_list m_errors;
  };
} // namespace yave