//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core.hpp>

#include <memory>

namespace yave {

  /// Node property of parsed_node_graph.
  class parsed_node_property
  {
  public:
    parsed_node_property(
      const object_ptr<const Object>& instance,
      const object_ptr<const Type>& type,
      const std::shared_ptr<const bind_info>& bind_info)
      : m_instance {instance}
      , m_type {type}
      , m_bind_info {bind_info}
    {
    }

    /// Get instance object.
    const object_ptr<const Object>& instance() const
    {
      return m_instance;
    }

    /// Get type.
    const object_ptr<const Type> type() const
    {
      return m_type;
    }

    /// Get bind info.
    const std::shared_ptr<const bind_info>& bind_info() const
    {
      return m_bind_info;
    }

    /// Get name fron bind info.
    const std::string& name() const
    {
      return m_bind_info->name();
    }

    /// Get output socket from bind info.
    const std::string& output_socket() const
    {
      return m_bind_info->output_socket();
    }

    /// Get list of input sockets from bind info.
    const std::vector<std::string>& input_sockets() const
    {
      return m_bind_info->input_sockets();
    }

  private:
    object_ptr<const Object> m_instance;
    object_ptr<const Type> m_type;
    std::shared_ptr<const class bind_info> m_bind_info;
  };
} // namespace yave