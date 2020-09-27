//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/compile_result.hpp>

#include <fmt/format.h>

namespace yave::compile_results {

  auto no_valid_overloading::message() const -> std::string
  {
    return fmt::format(
      "No valid overloading: socket={}", to_string(m_socket_id));
  }

  auto type_missmatch::message() const -> std::string
  {
    return fmt::format(
      "Type missmatch: expected={} s={}, provided={} s={}",
      to_string(m_expected),
      to_string(m_socket_expected_id),
      to_string(m_provided),
      to_string(m_socket_provided_id));
  }

  auto unsolvable_constraints::message() const -> std::string
  {
    return fmt::format(
      "Unsolvable constraint: t1={}, s1={}, t2={}, s2={}",
      to_string(m_lhs_type),
      to_string(m_lhs_id),
      to_string(m_rhs_type),
      to_string(m_rhs_id));
  }

  auto unexpected_error::message() const -> std::string
  {
    return fmt::format(
      "Unexpected compile error occured: socket={}, msg = {}",
      to_string(m_socket_id),
      m_msg);
  }
}