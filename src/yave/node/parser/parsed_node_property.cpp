//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/parsed_node_property.hpp>
#include <yave/node/support/socket_instance_manager.hpp>

namespace yave {

  parsed_node_property::parsed_node_property(
    const socket_instance* inst_ptr,
    bool is_root)
    : m_inst_ptr {inst_ptr}
    , m_is_root {is_root}
  {
  }

  const object_ptr<const Object>& parsed_node_property::instance() const
  {
    return m_inst_ptr->instance;
  }

  const object_ptr<const Type> parsed_node_property::type() const
  {
    return m_inst_ptr->type;
  }

  const std::shared_ptr<const bind_info>&
    parsed_node_property::bind_info() const
  {
    return m_inst_ptr->bind;
  }

  const std::string& parsed_node_property::name() const
  {
    return m_inst_ptr->bind->name();
  }

  const std::string& parsed_node_property::output_socket() const
  {
    return m_inst_ptr->bind->output_socket();
  }

  const std::vector<std::string>& parsed_node_property::input_sockets() const
  {
    return m_inst_ptr->bind->input_sockets();
  }

  bool parsed_node_property::is_root() const
  {
    return m_is_root;
  }

  void parsed_node_property::set_root()
  {
    m_is_root = true;
  }
} // namespace yave