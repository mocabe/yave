//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/graph_definition.hpp>
#include <yave/node/core/descriptor_handle.hpp>

namespace yave {

  /// Socket handle
  using socket_handle = descriptor_handle<graph_t::socket_descriptor_type>;

} // namespace yave