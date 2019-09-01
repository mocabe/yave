//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>
#include <yave/support/error.hpp>

#include <optional>

namespace yave {

  struct parsed_node_graph
  {
    /// node_graph generated by parser
    node_graph graph;
    /// root of parsed graph
    node_handle root;
  };

  /// Parse node graph and generate data for compiler stage.
  class node_parser
  {
  public:
    /// Default ctor.
    node_parser();

    /// Parser node tree.
    auto parse(const node_graph& input, const node_handle& root)
      -> std::optional<parsed_node_graph>;

    /// Get last errors.
    auto get_errors() const -> error_list;

  private:
    /// Extract prime tree.
    auto _extract(const node_graph& input, const node_handle& root)
      -> std::optional<parsed_node_graph>;
    /// Remove all syntax sugars.
    auto _desugar(parsed_node_graph&& parsed_graph)
      -> std::optional<parsed_node_graph>;
    /// Validate prime tree.
    auto _parse(parsed_node_graph&& parsed_graph)
      -> std::optional<parsed_node_graph>;

  private:
    auto _lock() const -> std::unique_lock<std::mutex>;

  private:
    mutable std::mutex m_mtx;

  private:
    error_list m_errors;
  };
} // namespace yave