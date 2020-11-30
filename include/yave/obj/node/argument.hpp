//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/node/argument.hpp>
#include <yave/rts/box.hpp>

namespace yave {

  /// node argument variable
  using NodeArgumentVariable = Box<node_argument_variable_object_value>;
  /// node default argument data
  using NodeArgument = Box<node_argument_object_value>;

} // namespace yave

YAVE_DECL_TYPE(yave::NodeArgumentVariable, "36dd1bfd-c798-47e0-9dca-3b12e9e2cf79");
YAVE_DECL_TYPE(yave::NodeArgument, "8cfff49e-b86b-4cca-8d9d-34800003ff3b");