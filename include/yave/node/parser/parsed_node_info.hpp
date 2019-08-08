//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/bind_info.hpp>

namespace yave {

  /// Node info of parsed_node_graph.
  class parsed_node_info
  {
  public:
    parsed_node_info()                        = default;
    parsed_node_info(const parsed_node_info&) = default;
    parsed_node_info(parsed_node_info&&)      = default;
    parsed_node_info& operator=(const parsed_node_info&) = default;
    parsed_node_info& operator=(parsed_node_info&&) = default;

    parsed_node_info(
      const object_ptr<const Object>& instance,
      const object_ptr<const Type>& type,
      const std::shared_ptr<const class bind_info>& bind_info);

    /// Get instance object.
    const object_ptr<const Object>& instance() const;

    /// Get current type of the node.
    const object_ptr<const Type>& type() const;

    /// Get current bind info of the node.
    std::shared_ptr<const class bind_info> bind_info() const;

    /// Get name of node.
    const std::string& name() const;

    /// Get output socket of the node.
    const std::string& output_socket() const;

    /// Get list of input sockets of the node.
    const std::vector<std::string>& input_sockets() const;

  private: /* from parsed_node_property */
    object_ptr<const Object> m_instance;
    object_ptr<const Type> m_type;
    std::shared_ptr<const class bind_info> m_bind_info;

  private: /* local */
    std::string m_name;
    std::string m_output_socket;
    std::vector<std::string> m_input_sockets;
  };

} // namespace yave