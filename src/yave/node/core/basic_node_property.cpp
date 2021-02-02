//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/basic_node_property.hpp>

namespace yave {

  basic_node_property::basic_node_property(
    const std::string& name,
    basic_node_type type)
    : m_name {name}
    , m_type {type}
    , m_data {std::nullopt}
    , m_flags {0}
  {
  }

  basic_node_property::basic_node_property(const basic_node_property& other)
    : m_name {other.m_name}
    , m_type {other.m_type}
    , m_data {other.m_data}
    , m_flags {0}
  {
  }

  auto basic_node_property::name() const -> const std::string&
  {
    return m_name;
  }

  bool basic_node_property::is_normal() const
  {
    return m_type == basic_node_type::normal;
  }

  bool basic_node_property::is_interface() const
  {
    return m_type == basic_node_type::interface;
  }

  basic_node_type basic_node_property::type() const
  {
    return m_type;
  }

  void basic_node_property::set_name(const std::string& new_name)
  {
    m_name = new_name;
  }

  bool basic_node_property::has_data() const
  {
    return m_data.has_value();
  }

  auto basic_node_property::get_data() const
    -> std::optional<object_ptr<Object>>
  {
    return m_data;
  }

  void basic_node_property::set_data(object_ptr<Object> data)
  {
    m_data = std::move(data);
  }

  auto basic_node_property::get_flags() const -> uint8_t
  {
    return m_flags;
  }

  void basic_node_property::set_flags(uint8_t bits) const
  {
    m_flags = bits;
  }

} // namespace yave