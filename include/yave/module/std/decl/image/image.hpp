//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    /// Image constructor
    class Image;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Image>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "Image",
        "std::media::image",
        "Load image form file",
        {"file"},
        {"image"});
    }
  };
}