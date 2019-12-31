//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/errors.hpp>
#include <fmt/format.h>

namespace yave {

  std::string parse_error::missing_input::message() const
  {
    return fmt::format(
      "Missing input connections: n={}, s={}",
      to_string(m_node_id),
      to_string(m_socket_id));
  }

  std::string parse_error::missing_output::message() const
  {
    return fmt::format(
      "Missing output connections: n={}, s={}",
      to_string(m_node_id),
      to_string(m_socket_id));
  }

  std::string parse_error::unexpected_error::message() const
  {
    return fmt::format("Unexpected parse error occured: msg={}", m_msg);
  }
}