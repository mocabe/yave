//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts.hpp>

#include <memory>

namespace yave {

  struct socket_instance;

  /// Node property of parsed_node_graph.
  class parsed_node_property
  {
    friend class parsed_node_graph;

  public:
    parsed_node_property(const socket_instance* inst_ptr, bool is_root = false);

    /// Get instance object.
    const object_ptr<const Object>& instance() const;

    /// Get type.
    const object_ptr<const Type> type() const;

    /// Get bind info.
    const std::shared_ptr<const class bind_info>& bind_info() const;

    /// Get name fron bind info.
    const std::string& name() const;

    /// Get output socket from bind info.
    const std::string& output_socket() const;

    /// Get list of input sockets from bind info.
    const std::vector<std::string>& input_sockets() const;

    /// root?
    bool is_root() const;

    /// set this node root
    void set_root();

  private:
    /// points to element of parsed_node_graph::m_instances
    const socket_instance* m_inst_ptr;
    /// root?
    bool m_is_root;
  };
} // namespace yave