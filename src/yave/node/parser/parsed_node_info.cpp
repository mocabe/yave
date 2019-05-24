//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/parsed_node_info.hpp>

namespace yave {

  parsed_node_info::parsed_node_info(
    const object_ptr<const Object>& instance,
    const object_ptr<const Type>& type,
    std::shared_ptr<const class bind_info>& bind_info)
    : m_instance {instance}
    , m_type {type}
    , m_bind_info {bind_info}
  {
    m_name          = m_bind_info->name();
    m_output_socket = m_bind_info->output_socket();
    m_input_sockets = m_bind_info->input_sockets();
  }

  const object_ptr<const Object>& parsed_node_info::instance() const
  {
    return m_instance;
  }

  const object_ptr<const Type>& parsed_node_info::type() const
  {
    return m_type;
  }

  std::shared_ptr<const class bind_info> parsed_node_info::bind_info() const
  {
    return m_bind_info;
  }

  const std::string& parsed_node_info::name() const
  {
    return m_name;
  }

  const std::string& parsed_node_info::output_socket() const
  {
    return m_output_socket;
  }

  const std::vector<std::string>& parsed_node_info::input_sockets() const
  {
    return m_input_sockets;
  }

} // namespace yave