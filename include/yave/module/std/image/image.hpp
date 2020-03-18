//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/lib/image_buffer/image_buffer_manager.hpp>

namespace yave {

  namespace node {
    /// Image constructor
    class Image;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Image);

  template <>
  struct node_definition_traits<node::Image, modules::_std::tag>
  {
    static auto get_node_definitions(image_buffer_manager& mngr)
      -> std::vector<node_definition>;
  };
}