//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/managed_node_graph.hpp>
#include <yave/support/error.hpp>

#include <optional>

namespace yave {

  /// Parse node graph and generate data for compiler stage.
  /// Parser extracts prime tree which can be passed to compiler stage.
  /// Checks all insufficient inputs and removes intermediate nodes.
  class node_parser
  {
  public:
    /// Default ctor.
    node_parser();

    /// Parser node tree.
    [[nodiscard]] auto parse(const managed_node_graph& graph)
      -> std::optional<managed_node_graph>;

    /// Get last errors.
    [[nodiscard]] auto get_errors() const -> error_list;

  private:
    /// Omit unused trees, etc.
    [[nodiscard]] auto _extract(const managed_node_graph& graph)
      -> std::optional<managed_node_graph>;

    /// Validate prime tree.
    [[nodiscard]] auto _validate(managed_node_graph&& graph)
      -> std::optional<managed_node_graph>;

  private:
    [[nodiscard]] auto _lock() const -> std::unique_lock<std::mutex>;

  private:
    mutable std::mutex m_mtx;

  private:
    error_list m_errors;
  };
} // namespace yave