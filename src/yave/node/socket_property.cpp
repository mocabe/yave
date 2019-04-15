//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/socket_property.hpp>

namespace yave {

  SocketProperty::SocketProperty(const std::string& name, input_t)
    : m_name {name}
    , m_io {input_v}
  {
  }

  SocketProperty::SocketProperty(const std::string& name, output_t)
    : m_name {name}
    , m_io {output_v}
  {
  }

  bool SocketProperty::is_input() const
  {
    return m_io == input_v;
  }

  bool SocketProperty::is_output() const
  {
    return m_io == output_v;
  }

  const std::string& SocketProperty::name() const
  {
    return m_name;
  }

} // namespace yave