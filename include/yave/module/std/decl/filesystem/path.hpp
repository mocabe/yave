//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/decl/prim/primitive.hpp>

namespace yave {

  namespace node {
    /// File path ctor
    class FilePath;
  } // namespace node

  template <>
  struct node_declaration_traits<node::FilePath>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "FilePath",
        "std::filesystem",
        "File path constructor",
        {"path"},
        {"path"},
        {{0, make_data_type_holder<String>()}});
    }
  };
}