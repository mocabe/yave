//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/graph_definition.hpp>
#include <yave/node/descriptor_handle.hpp>

namespace yave {

  /// Node handle
  using NodeHandle = DescriptorHandle<graph_t::node_descriptor_type>;

} // namespace yave