//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/parsed_socket_property.hpp>

namespace yave {

  parsed_socket_property::parsed_socket_property(
    const std::string& name,
    input_t)
    : m_name {name}
    , m_is_input {true}
  {
  }

  parsed_socket_property::parsed_socket_property(
    const std::string& name,
    output_t)
    : m_name {name}
    , m_is_input {false}
  {
  }

  bool parsed_socket_property::is_input() const
  {
    return m_is_input;
  }

  bool parsed_socket_property::is_output() const
  {
    return !m_is_input;
  }

  const std::string& parsed_socket_property::name() const
  {
    return m_name;
  }

} // namespace yave