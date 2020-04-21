//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_info.hpp>
#include <yave/lib/vec/tvec2.hpp>

namespace yave {

  /// node type
  enum class node_type2
  {
    group,
    group_input,
    group_output,
    function,
  };

  /// node call type
  enum class node_call_type
  {
    call,       //< normal call
    definition, //< definition point
  };

  /// node_info for node graph
  class node_info2
  {
  public:
    node_info2()                  = delete;
    node_info2(const node_info2&) = default;
    node_info2(node_info2&&)      = default;
    node_info2& operator=(const node_info2&) = default;
    node_info2& operator=(node_info2&&) = default;

    node_info2(
      std::string name,
      std::vector<socket_handle> input_sockets,
      std::vector<socket_handle> output_sockets,
      node_type2 type,
      node_call_type call_type,
      tvec2<float> pos)
      : m_name {name}
      , m_input_sockets {input_sockets}
      , m_output_sockets {output_sockets}
      , m_type {type}
      , m_call_type {call_type}
      , m_pos {pos}
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

    /// normal?
    [[nodiscard]] bool is_function() const
    {
      return m_type == node_type2::function;
    }

    /// group?
    [[nodiscard]] bool is_group() const
    {
      return m_type == node_type2::group;
    }

    /// group input?
    [[nodiscard]] bool is_group_input() const
    {
      return m_type == node_type2::group_input;
    }

    /// group output?
    [[nodiscard]] bool is_group_output() const
    {
      return m_type == node_type2::group_output;
    }

    /// type
    [[nodiscard]] auto& type() const
    {
      return m_type;
    }

    /// definition?
    [[nodiscard]] bool is_definition() const
    {
      return m_call_type == node_call_type::definition;
    }

    /// call type
    [[nodiscard]] auto& call_type() const
    {
      return m_call_type;
    }

    /// pos
    [[nodiscard]] auto& pos() const
    {
      return m_pos;
    }

  private:
    /// name of node
    std::string m_name;
    /// list of input sockets
    std::vector<socket_handle> m_input_sockets;
    /// list of output sockets
    std::vector<socket_handle> m_output_sockets;
    /// node type
    node_type2 m_type;
    /// definition
    node_call_type m_call_type;
    /// pos
    tvec2<float> m_pos;
  };
}