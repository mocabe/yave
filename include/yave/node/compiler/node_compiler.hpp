//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/compiler/executable.hpp>
#include <yave/node/compiler/errors.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/core/socket_instance_manager.hpp>
#include <yave/node/core/node_definition_store.hpp>
#include <yave/node/core/node_declaration_store.hpp>

namespace yave {

  class node_compiler
  {
  public:
    /// Ctor
    node_compiler();

    /// Compile parsed graph
    [[nodiscard]] auto compile(
      managed_node_graph&& graph,
      const node_definition_store& defs) -> std::optional<executable>;

    /// Get last errors
    [[nodiscard]] auto get_errors() const -> error_list;

  private:
    /// Optimize parsed graph
    [[nodiscard]] auto _optimize_early(managed_node_graph&& graph)
      -> std::optional<managed_node_graph>;

    /// Resolve overloadings and check type
    [[nodiscard]] auto _type(
      const managed_node_graph& graph,
      const node_definition_store& defs) -> std::optional<executable>;

    /// Verbose type check
    [[nodiscard]] auto _verbose_check(
      const executable& executable,
      const managed_node_graph& graph) -> bool;

    /// Optimize executable
    [[nodiscard]] auto _optimize(
      executable&& exe,
      const managed_node_graph& graph) -> executable;

  private:
    error_list m_errors;
  };

} // namespace yave