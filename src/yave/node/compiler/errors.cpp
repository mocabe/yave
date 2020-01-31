//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/errors.hpp>
#include <yave/rts/to_string.hpp>
#include <fmt/format.h>

namespace yave::compile_error {

  std::string no_valid_overloading::message() const
  {
    return fmt::format("No valid overloading: node={}", to_string(m_node.id()));
  }

  std::string type_missmatch::message() const
  {
    return fmt::format(
      "Type missmatch: node={}, expected={}, provided={}",
      to_string(m_node.id()),
      to_string(m_expected),
      to_string(m_provided));
  }

  std::string unexpected_error::message() const
  {
    return fmt::format(
      "Unexpected compile error occured: node={}, msg = {}",
      to_string(m_node.id()),
      m_msg);
  }
}