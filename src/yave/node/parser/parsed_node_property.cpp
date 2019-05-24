//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/parsed_node_property.hpp>

namespace yave {

  parsed_node_property::parsed_node_property(
    const object_ptr<const Object>& instance,
    const object_ptr<const Type>& type,
    const std::shared_ptr<const class bind_info>& bind_info)
    : m_instance {instance}
    , m_type {type}
    , m_bind_info {bind_info}
  {
  }

  const object_ptr<const Object>& parsed_node_property::instance() const
  {
    return m_instance;
  }

  const object_ptr<const Type> parsed_node_property::type() const
  {
    return m_type;
  }

  const std::shared_ptr<const bind_info>&
    parsed_node_property::bind_info() const
  {
    return m_bind_info;
  }

  const std::string& parsed_node_property::name() const
  {
    return m_bind_info->name();
  }

  const std::string& parsed_node_property::output_socket() const
  {
    return m_bind_info->output_socket();
  }

  const std::vector<std::string>& parsed_node_property::input_sockets() const
  {
    return m_bind_info->input_sockets();
  }
} // namespace yave