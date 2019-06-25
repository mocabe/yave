//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/parser/parsed_node_graph.hpp>
#include <yave/node/compiler/executable.hpp>

namespace yave {

  class node_compiler
  {
  public:
    /// Ctor
    node_compiler(const parsed_node_graph& parsed_graph);
    /// Generate executable from parsed node graph.
    executable compile(const parsed_node_handle& root) const;

    /// Lock.
    std::unique_lock<std::mutex> lock() const;

  private:
    const parsed_node_graph& m_parsed_graph;

  private:
    /// mutex
    mutable std::mutex m_mtx;
  };
} // namespace yave