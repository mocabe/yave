//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/exception.hpp>

namespace yave::parse_error {

  parse_error::parse_error(const std::string& message, const node_handle& h)
    : std::logic_error("parse_error: " + message)
    , m_node {h}
  {
  }

  parse_error::parse_error(const char* message, const node_handle& h)
    : std::logic_error("parse_error: " + std::string(message))
    , m_node {h}
  {
  }

  node_handle parse_error::node() const noexcept
  {
    return m_node;
  }

  no_overloading::no_overloading(
    const std::string& msg,
    const node_handle& h,
    const object_ptr<const Type>& tp)
    : parse_error(msg, h)
    , m_type {tp}
  {
  }

  object_ptr<const Type> no_overloading::type() const
  {
    return m_type;
  }

  ambiguous_overloading::ambiguous_overloading(
    const std::string& msg,
    const object_ptr<const Type>& tp,
    const node_handle& h)
    : parse_error(msg, h)
    , m_type {tp}
  {
  }

  object_ptr<const Type> ambiguous_overloading::type() const
  {
    return m_type;
  }

  root_type_missmatch::root_type_missmatch(
    const std::string& msg,
    const node_handle& h,
    const object_ptr<const Type>& expected,
    const object_ptr<const Type>& provided)
    : parse_error(msg, h)
    , m_expected {expected}
    , m_provided {provided}
  {
  }

  object_ptr<const Type> root_type_missmatch::expected() const
  {
    return m_expected;
  }

  object_ptr<const Type> root_type_missmatch::provided() const
  {
    return m_provided;
  }

} // namespace yave::parse_error