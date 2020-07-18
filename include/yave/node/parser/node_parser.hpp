//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/structured_node_graph.hpp>
#include <yave/support/error.hpp>

#include <optional>

namespace yave {

  /// parse result
  struct node_parser_result
  {
    /// Parsed node graph
    std::optional<structured_node_graph> node_graph = std::nullopt;
    /// Error list
    error_list errors = {};

    /// success check by bool conversion
    operator bool() const
    {
      return node_graph.has_value();
    }
  };

  /// Parse node graph and generate data for compiler stage.
  class node_parser
  {
  public:
    /// Default ctor.
    node_parser();
    /// Dtor
    ~node_parser() noexcept;

    /// Parse node graph
    /// \param ng node graph to parse
    /// \param out output socket of the node graph. must be output socket of
    /// node group.
    [[nodiscard]] auto parse(
      structured_node_graph&& ng,
      const socket_handle& out) -> node_parser_result;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
} // namespace yave