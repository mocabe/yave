//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  /// Layer compositor node
  struct LayerCompositor;

  // info traits
  template <>
  struct node_info_traits<LayerCompositor>
  {
    static node_info get_node_info()
    {
      return node_info("LayerCompositor", {"src", "dst", "blend op"}, {"out"});
    }
  };

} // namespace yave