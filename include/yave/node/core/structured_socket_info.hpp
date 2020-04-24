//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/socket_info.hpp>

namespace yave {

  /// socket info
  class structured_socket_info
  {
  public:
    structured_socket_info()                              = delete;
    structured_socket_info(const structured_socket_info&) = default;
    structured_socket_info(structured_socket_info&&)      = default;
    structured_socket_info& operator=(const structured_socket_info&) = default;
    structured_socket_info& operator=(structured_socket_info&&) = default;

    structured_socket_info(std::string name, socket_type type, node_handle node)
      : m_name {name}
      , m_type {type}
      , m_node {node}
    {
    }

    /// name
    [[nodiscard]] auto& name() const
    {
      return m_name;
    }

    /// input?
    [[nodiscard]] bool is_input() const
    {
      return m_type == socket_type::input;
    }

    /// output?
    [[nodiscard]] bool is_output() const
    {
      return m_type == socket_type::output;
    }

    /// socket type
    [[nodiscard]] auto& type() const
    {
      return m_type;
    }

    /// Get node
    [[nodiscard]] auto& node() const
    {
      return m_node;
    }

  private:
    std::string m_name;
    socket_type m_type;
    node_handle m_node;
  };

} // namespace yave