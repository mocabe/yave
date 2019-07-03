//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/node/core/node_handle.hpp>

namespace yave {

  /// Alpha blend op
  enum class blend_operation : uint32_t
  {
    src   = 2,
    dst   = 3,
    over  = 4,
    in    = 5,
    out   = 6,
    add   = 7,
  };

  /// to_string
  constexpr const char* to_string(blend_operation op)
  {
    switch (op) {
      case blend_operation::src:
        return "blend_operation::src";
      case blend_operation::dst:
        return "blend_operation::dst";
      case blend_operation::over:
        return "blend_operation::over";
      case blend_operation::in:
        return "blend_operation::in";
      case blend_operation::out:
        return "blend_operation::out";
      case blend_operation::add:
        return "blend_operation::add";
    }
    return "(unrecognized blend operation)";
  }

  /// Layer composition
  struct layer_composition
  {
    /// Image input
    node_handle input;
    /// Handle to compositor node
    node_handle compositor;
    /// Handle to blend op
    node_handle blend_op;
  };
}