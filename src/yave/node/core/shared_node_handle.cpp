//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/shared_node_handle.hpp>
#include <yave/node/core/managed_node_graph.hpp>

namespace yave {

  shared_node_handle::_data::_data(managed_node_graph& g, const node_handle& h)
    : graph {g}
    , handle {h}
  {
    if (!g.exists(h))
      throw std::runtime_error("Invalid node handle");
  }

  shared_node_handle::_data::~_data()
  {
    if (handle)
      graph.destroy(handle);
  }

} // namespace yave