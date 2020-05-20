//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/structured_node_graph.hpp>
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
    /// Dtor
    ~node_parser() noexcept;

    /// Parse node graph
    [[nodiscard]] auto parse(structured_node_graph&& ng)
      -> std::optional<structured_node_graph>;

    /// Get last errors.
    [[nodiscard]] auto get_errors() const -> error_list;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
} // namespace yave