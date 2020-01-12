//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_handle.hpp>

namespace yave::app {

  /// Editor data for node
  struct editor_node_info
  {
    /// node handle
    node_handle handle;

    /// selected?
    bool is_selected;

    /// hovered?
    bool is_hovered;

    /// position
    tvec2<float> position;
  };
} 