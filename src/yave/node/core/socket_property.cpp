//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/socket_property.hpp>

#include <yave/rts/utility.hpp>

namespace yave {

  socket_property::socket_property(const std::string& name, socket_type type)
    : m_name {name}
    , m_type {type}
    , m_data {std::nullopt}
  {
  }

  socket_property::socket_property(const socket_property& other)
    : m_name {other.m_name}
    , m_type {other.m_type}
    , m_data {other.m_data}
  {
    if (m_data)
      *m_data = clone(*m_data);
  }

  bool socket_property::is_input() const
  {
    return m_type == socket_type::input;
  }

  bool socket_property::is_output() const
  {
    return m_type == socket_type::output;
  }

  auto socket_property::type() const -> socket_type
  {
    return m_type;
  }

  auto socket_property::name() const -> const std::string&
  {
    return m_name;
  }

  bool socket_property::has_data() const
  {
    return m_data.has_value();
  }

  auto socket_property::get_data() const -> std::optional<object_ptr<Object>>
  {
    return m_data;
  }

  void socket_property::set_data(object_ptr<Object> data)
  {
    m_data = data;
  }

  void socket_property::set_name(std::string new_name)
  {
    m_name = std::move(new_name);
  }
} // namespace yave