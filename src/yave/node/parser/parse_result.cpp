//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/parse_result.hpp>
#include <fmt/format.h>

namespace yave {

  std::string parse_results::missing_input::message() const
  {
    return fmt::format(
      "Missing input connections: n={}, s={}",
      to_string(m_node_id),
      to_string(m_socket_id));
  }

  std::string parse_results::missing_output::message() const
  {
    return fmt::format(
      "Missing output connections: n={}, s={}",
      to_string(m_node_id),
      to_string(m_socket_id));
  }

  std::string parse_results::unexpected_error::message() const
  {
    return fmt::format("Unexpected parse error occured: msg={}", m_msg);
  }
}