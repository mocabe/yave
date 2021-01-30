//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_info.hpp>

#include <stdexcept>
#include <range/v3/algorithm.hpp>

namespace yave {

  node_info::node_info(
    std::string name,
    std::vector<socket_handle> input_sockets,
    std::vector<socket_handle> output_sockets,
    node_type type)
    : m_name {std::move(name)}
    , m_input_sockets {std::move(input_sockets)}
    , m_output_sockets {std::move(output_sockets)}
    , m_type {type}
  {
  }

  auto node_info::name() const -> const std::string&
  {
    return m_name;
  }

  auto node_info::type() const -> node_type
  {
    return m_type;
  }

  void node_info::set_name(std::string name)
  {
    m_name = std::move(name);
  }

  auto node_info::sockets(socket_type type) const
    -> const std::vector<socket_handle>&
  {
    switch (type) {
      case socket_type::input:
        return m_input_sockets;
      case socket_type::output:
        return m_output_sockets;
    }
  }

} // namespace yave