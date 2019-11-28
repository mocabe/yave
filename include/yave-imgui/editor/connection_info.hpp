//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/connection_handle.hpp>
#include <yave/lib/imgui/extension.hpp>

namespace yave::editor::imgui {

  /// Editor connection info
  struct editor_connection_info
  {
    /// connection
    connection_handle handle;

    /// selected?
    bool is_selected;

    /// hovered?
    bool is_hovered;
  };
} // namespace yave