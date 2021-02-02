//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/basic_node_info.hpp>

#include <stdexcept>
#include <range/v3/algorithm.hpp>

namespace yave {

  basic_node_info::basic_node_info(
    std::string name,
    std::vector<socket_handle> input_sockets,
    std::vector<socket_handle> output_sockets,
    basic_node_type type)
    : m_name {std::move(name)}
    , m_input_sockets {std::move(input_sockets)}
    , m_output_sockets {std::move(output_sockets)}
    , m_type {type}
  {
  }

  auto basic_node_info::name() const -> const std::string&
  {
    return m_name;
  }

  auto basic_node_info::type() const -> basic_node_type
  {
    return m_type;
  }

  void basic_node_info::set_name(std::string name)
  {
    m_name = std::move(name);
  }

  auto basic_node_info::sockets(basic_socket_type type) const
    -> const std::vector<socket_handle>&
  {
    switch (type) {
      case basic_socket_type::input:
        return m_input_sockets;
      case basic_socket_type::output:
        return m_output_sockets;
    }
    unreachable();
  }

} // namespace yave