//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/errors.hpp>
#include <fmt/format.h>

namespace yave {

  std::string parse_error::no_sufficient_input::message() const
  {
    return fmt::format(
      "No sufficient input connections on non primitive node: id={}",
      m_node.id().data);
  }

  std::string parse_error::unexpected_error::message() const
  {
    return fmt::format("Unexpected parse error occured: msg={}", m_msg);
  }
}