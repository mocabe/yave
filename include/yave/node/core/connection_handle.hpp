//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/descriptor_handle.hpp>

namespace yave {

  using opaque_edge_descriptor_type = struct opaque_edge_type *;

  /// Handle of socket connection
  using connection_handle = descriptor_handle<opaque_edge_descriptor_type>;

} // namespace yave