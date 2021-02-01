//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/descriptor_handle.hpp>

namespace yave {

  using opaque_socket_descriptor_type = struct opaque_socket_type *;

  /// Socket handle
  using socket_handle = descriptor_handle<opaque_socket_descriptor_type>;

} // namespace yave