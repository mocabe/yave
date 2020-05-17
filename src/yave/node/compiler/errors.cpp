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
    return fmt::format(
      "No valid overloading: socket={}", to_string(m_socket.id()));
  }

  std::string type_missmatch::message() const
  {
    return fmt::format(
      "Type missmatch: expected={} s={}, provided={} s={}",
      to_string(m_expected),
      to_string(m_socket_expected.id()),
      to_string(m_provided),
      to_string(m_socket_provided.id()));
  }

  std::string unexpected_error::message() const
  {
    return fmt::format(
      "Unexpected compile error occured: socket={}, msg = {}",
      to_string(m_socket.id()),
      m_msg);
  }
} // namespace yave::compile_error