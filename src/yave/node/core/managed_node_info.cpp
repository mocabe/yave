//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/managed_node_info.hpp>

namespace yave {

  managed_node_info::managed_node_info(
    std::string name,
    std::vector<std::string> input_sockets,
    std::vector<std::string> output_sockets,
    managed_node_type type)
    : m_name {std::move(name)}
    , m_input_sockets {std::move(input_sockets)}
    , m_output_sockets {std::move(output_sockets)}
    , m_type {type}
  {
  }

  auto managed_node_info::name() const -> const std::string&
  {
    return m_name;
  }

  auto managed_node_info::input_sockets() const
    -> const std::vector<std::string>&
  {
    return m_input_sockets;
  }

  auto managed_node_info::output_sockets() const
    -> const std::vector<std::string>&
  {
    return m_output_sockets;
  }

  auto managed_node_info::type() const -> managed_node_type
  {
    return m_type;
  }

  bool managed_node_info::is_normal() const
  {
    return m_type == managed_node_type::normal;
  }

  bool managed_node_info::is_primitive() const
  {
    return m_type == managed_node_type::primitive;
  }

  bool managed_node_info::is_group() const
  {
    return m_type == managed_node_type::group;
  }

  bool managed_node_info::is_group_input() const
  {
    return m_type == managed_node_type::group_input;
  }

  bool managed_node_info::is_group_output() const
  {
    return m_type == managed_node_type::group_output;
  }
} // namespace yave