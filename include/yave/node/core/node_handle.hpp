//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/descriptor_handle.hpp>

namespace yave {

  using opaque_node_descriptor_type = struct opaque_node_type *;

  /// Node handle
  using node_handle = descriptor_handle<opaque_node_descriptor_type>;

} // namespace yave