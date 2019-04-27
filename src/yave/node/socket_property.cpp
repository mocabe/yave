//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/socket_property.hpp>

namespace yave {

  SocketProperty::SocketProperty(const std::string& name, input_t)
    : m_name {name}
    , m_is_input {true}
  {
  }

  SocketProperty::SocketProperty(const std::string& name, output_t)
    : m_name {name}
    , m_is_input {false}
  {
  }

  bool SocketProperty::is_input() const
  {
    return m_is_input;
  }

  bool SocketProperty::is_output() const
  {
    return !m_is_input;
  }

  const std::string& SocketProperty::name() const
  {
    return m_name;
  }

} // namespace yave