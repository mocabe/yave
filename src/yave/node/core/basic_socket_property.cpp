//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/basic_socket_property.hpp>

namespace yave {

  basic_socket_property::basic_socket_property(
    std::string name,
    basic_socket_type type)
    : m_name {std::move(name)}
    , m_type {type}
    , m_data {std::nullopt}
  {
  }

  basic_socket_property::basic_socket_property(
    const basic_socket_property& other)
    : m_name {other.m_name}
    , m_type {other.m_type}
    , m_data {other.m_data}
  {
  }

  bool basic_socket_property::is_input() const
  {
    return m_type == basic_socket_type::input;
  }

  bool basic_socket_property::is_output() const
  {
    return m_type == basic_socket_type::output;
  }

  auto basic_socket_property::type() const -> basic_socket_type
  {
    return m_type;
  }

  auto basic_socket_property::name() const -> const std::string&
  {
    return m_name;
  }

  bool basic_socket_property::has_data() const
  {
    return m_data.has_value();
  }

  auto basic_socket_property::get_data() const
    -> std::optional<object_ptr<Object>>
  {
    return m_data;
  }

  void basic_socket_property::set_data(object_ptr<Object> data)
  {
    m_data = data;
  }

  void basic_socket_property::set_name(std::string new_name)
  {
    m_name = std::move(new_name);
  }
} // namespace yave