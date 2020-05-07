//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/structured_node_graph.hpp>

#include <chrono>
#include <functional>

namespace yave::editor {

  struct editor_data
  {
    auto& node_graph()
    {
      return m_node_graph;
    }

    auto& node_graph() const
    {
      return m_node_graph;
    }

  private:
    structured_node_graph m_node_graph;
  };

} // namespace yave::editor