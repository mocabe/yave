//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/errors.hpp>
#include <yave/rts/to_string.hpp>
#include <fmt/format.h>

namespace yave::compile_error {

  auto no_valid_overloading::message() const -> std::string
  {
    return fmt::format(
      "No valid overloading: socket={}", to_string(m_socket.id()));
  }

  auto type_missmatch::message() const -> std::string
  {
    return fmt::format(
      "Type missmatch: expected={} s={}, provided={} s={}",
      to_string(m_expected),
      to_string(m_socket_expected.id()),
      to_string(m_provided),
      to_string(m_socket_provided.id()));
  }

  auto unsolvable_constraints::message() const -> std::string
  {
    return fmt::format(
      "Unsolvable constraint: t1={}, s1={}, t2={}, s2={}",
      to_string(m_t1),
      to_string(m_s1.id()),
      to_string(m_t2),
      to_string(m_s2.id()));
  }

  auto unexpected_error::message() const -> std::string
  {
    return fmt::format(
      "Unexpected compile error occured: socket={}, msg = {}",
      to_string(m_socket.id()),
      m_msg);
  }
} // namespace yave::compile_error