//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/socket_property.hpp>

namespace yave {

  socket_property::socket_property(const std::string& name, socket_type type)
    : m_name {name}
    , m_type {type}
  {
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

  void socket_property::set_name(const std::string& new_name)
  {
    m_name = new_name;
  }
} // namespace yave