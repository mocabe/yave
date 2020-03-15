//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_property.hpp>

namespace yave {

  node_property::node_property(const std::string& name, node_type type)
    : m_name {name}
    , m_type {type}
    , m_data {std::nullopt}
    , m_flags {0}
  {
  }

  node_property::node_property(const node_property& other)
    : m_name {other.m_name}
    , m_type {other.m_type}
    , m_data {other.m_data}
    , m_flags {0}
  {
  }

  auto node_property::name() const -> const std::string&
  {
    return m_name;
  }

  bool node_property::is_normal() const
  {
    return m_type == node_type::normal;
  }

  bool node_property::is_interface() const
  {
    return m_type == node_type::interface;
  }

  node_type node_property::type() const
  {
    return m_type;
  }

  void node_property::set_name(const std::string& new_name)
  {
    m_name = new_name;
  }

  bool node_property::has_data() const
  {
    return m_data.has_value();
  }

  auto node_property::get_data() const -> std::optional<object_ptr<Object>>
  {
    return m_data;
  }

  void node_property::set_data(object_ptr<Object> data)
  {
    m_data = std::move(data);
  }

  auto node_property::get_flags() const -> uint8_t
  {
    return m_flags;
  }

  void node_property::set_flags(uint8_t bits) const
  {
    m_flags = bits;
  }

} // namespace yave