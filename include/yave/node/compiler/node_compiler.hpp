//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core.hpp>
#include <yave/node/support.hpp>
#include <yave/node/parser/parsed_node_graph.hpp>
#include <yave/node/compiler/executable.hpp>

namespace yave {

  class node_compiler
  {
  public:
    /// Ctor
    node_compiler() = default;
    /// Generate executable from parsed node graph.
    Executable compile(
      const parsed_node_graph& parsed_graph,
      const parsed_node_handle& root) const;
  };
} // namespace yave