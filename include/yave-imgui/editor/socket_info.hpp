//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/socket_handle.hpp>

namespace yave::editor::imgui {

  struct editor_socket_info
  {
    /// handle
    socket_handle handle;

    /// selected?
    bool is_selected;

    /// hovered?
    bool is_hovered;
  };
} // namespace yave::editor::imgui