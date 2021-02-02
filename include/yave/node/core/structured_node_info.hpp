//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/basic_node_info.hpp>

namespace yave {

  /// node type
  enum class structured_node_type
  {
    group,
    group_input,
    group_output,
    function,
    macro,
  };

  /// node call type
  enum class structured_call_type
  {
    call,       //< normal call
    definition, //< definition point
  };

  /// node info
  class structured_node_info
  {
  public:
    structured_node_info()                            = delete;
    structured_node_info(const structured_node_info&) = default;
    structured_node_info(structured_node_info&&)      = default;
    structured_node_info& operator=(const structured_node_info&) = default;
    structured_node_info& operator=(structured_node_info&&) = default;

    structured_node_info(
      std::string name,
      std::vector<socket_handle> input_sockets,
      std::vector<socket_handle> output_sockets,
      structured_node_type type,
      structured_call_type call_type)
      : m_name {name}
      , m_input_sockets {input_sockets}
      , m_output_sockets {output_sockets}
      , m_type {type}
      , m_call_type {call_type}
    {
    }

    /// name
    [[nodiscard]] auto& name() const
    {
      return m_name;
    }

    /// input sockets
    [[nodiscard]] auto& input_sockets() const
    {
      return m_input_sockets;
    }

    /// output sockets
    [[nodiscard]] auto& output_sockets() const
    {
      return m_output_sockets;
    }

    /// function?
    [[nodiscard]] bool is_function() const
    {
      return m_type == structured_node_type::function;
    }

    /// macro?
    [[nodiscard]] bool is_macro() const
    {
      return m_type == structured_node_type::macro;
    }

    /// group?
    [[nodiscard]] bool is_group() const
    {
      return m_type == structured_node_type::group;
    }

    /// group input?
    [[nodiscard]] bool is_group_input() const
    {
      return m_type == structured_node_type::group_input;
    }

    /// group output?
    [[nodiscard]] bool is_group_output() const
    {
      return m_type == structured_node_type::group_output;
    }

    /// type
    [[nodiscard]] auto& type() const
    {
      return m_type;
    }

    /// definition?
    [[nodiscard]] bool is_definition() const
    {
      return m_call_type == structured_call_type::definition;
    }

    /// call?
    [[nodiscard]] bool is_call() const
    {
      return m_call_type == structured_call_type::call;
    }

    /// call type
    [[nodiscard]] auto& call_type() const
    {
      return m_call_type;
    }

  private:
    /// name of node
    std::string m_name;
    /// list of input sockets
    std::vector<socket_handle> m_input_sockets;
    /// list of output sockets
    std::vector<socket_handle> m_output_sockets;
    /// node type
    structured_node_type m_type;
    /// definition
    structured_call_type m_call_type;
  };
} // namespace yave