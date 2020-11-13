//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/socket_info.hpp>
#include <yave/node/core/connection_handle.hpp>

namespace yave {

  /// socket type
  enum class structured_socket_type
  {
    input,
    output,
  };

  /// socket info
  class structured_socket_info
  {
  public:
    structured_socket_info()                              = delete;
    structured_socket_info(const structured_socket_info&) = default;
    structured_socket_info(structured_socket_info&&)      = default;
    structured_socket_info& operator=(const structured_socket_info&) = default;
    structured_socket_info& operator=(structured_socket_info&&) = default;

    structured_socket_info(
      std::string name,
      structured_socket_type type,
      node_handle node,
      size_t index,
      std::vector<connection_handle> cs)
      : m_name {std::move(name)}
      , m_type {type}
      , m_node {node}
      , m_index {index}
      , m_cs {std::move(cs)}
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
      return m_type == structured_socket_type::input;
    }

    /// output?
    [[nodiscard]] bool is_output() const
    {
      return m_type == structured_socket_type::output;
    }

    /// socket type
    [[nodiscard]] auto& type() const
    {
      return m_type;
    }

    /// node
    [[nodiscard]] auto& node() const
    {
      return m_node;
    }

    /// index
    [[nodiscard]] auto& index() const
    {
      return m_index;
    }

    /// connections
    [[nodiscard]] auto& connections() const
    {
      return m_cs;
    }

  private:
    std::string m_name;
    structured_socket_type m_type;
    node_handle m_node;
    size_t m_index;
    std::vector<connection_handle> m_cs;
  };

} // namespace yave