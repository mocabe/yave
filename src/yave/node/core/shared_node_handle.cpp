//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/shared_node_handle.hpp>
#include <yave/node/core/managed_node_graph.hpp>

namespace yave {

  shared_node_handle::_data::_data(managed_node_graph& g, const node_handle& h)
    : graph {g}
    , handle {h}
  {
    assert(g.exists(h));
  }

  shared_node_handle::_data::~_data()
  {
    if (handle)
      graph.destroy(handle);
  }

} // namespace yave