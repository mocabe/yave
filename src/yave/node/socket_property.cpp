//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/socket_property.hpp>

namespace yave {

  socket_property::socket_property(const std::string& name, input_t)
    : m_name {name}
    , m_is_input {true}
  {
  }

  socket_property::socket_property(const std::string& name, output_t)
    : m_name {name}
    , m_is_input {false}
  {
  }

  bool socket_property::is_input() const
  {
    return m_is_input;
  }

  bool socket_property::is_output() const
  {
    return !m_is_input;
  }

  const std::string& socket_property::name() const
  {
    return m_name;
  }

} // namespace yave