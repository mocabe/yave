//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    /// File path ctor
    class FilePath;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::FilePath);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::FilePath, modules::_std::tag);
}