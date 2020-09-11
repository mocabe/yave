//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/structured_node_graph.hpp>
#include <yave/obj/vec/vec.hpp>
#include <yave/lib/vector/vector.hpp>
#include <yave/lib/string/string.hpp>

namespace yave {

  enum class interface_node_type : uint32_t
  {
    group        = 0,
    group_input  = 1,
    group_output = 2,
    fucntion     = 3,
  };

  enum class interface_node_call : uint32_t
  {
    call       = 0,
    definition = 1,
  };

  enum class interface_socket_type : uint32_t
  {
    input  = 1,
    output = 2,
  };

  struct node_info_object_value
  {
    node_info_object_value(const structured_node_info& info)
      : m_name {info.name()}
    {
    }

  private:
    /// name
    string m_name;
    /// socket ids
    vector<uint64_t> m_input_sockets;
    /// socket ids
    vector<uint64_t> m_output_sockets;
    /// node type
    interface_node_type m_type;
    /// call type
    interface_node_call m_call;
    /// position
    dvec2 m_pos;
  };

  /// node info
  using NodeInfo = Box<node_info_object_value>;

  struct socket_info_object_value
  {
    /// name
    string m_name;
    /// node
    uint64_t m_node;
    /// connections
    vector<uint64_t> m_connections;
    /// socket type
    interface_socket_type m_socket_type;
  };

  using SocketInfo = Box<socket_info_object_value>;

  struct connection_info_object_value
  {
    /// src node
    uint64_t m_src_node;
    /// src socket
    uint64_t m_src_socket;
    /// dst node
    uint64_t m_dst_node;
    /// dst socket
    uint64_t m_dst_socket;
  };

  using ConnectionInfo = Box<connection_info_object_value>;

  /// general API of node graph
  struct node_graph_interface_object_value
  {
  public:
  private:
    void* m_handle;

  private:
    /// exists
    auto (*m_n_exists)(void* handle, uint64_t id) -> object_ptr<Bool>;
    auto (*m_s_exists)(void* handle, uint64_t id) -> object_ptr<Bool>;
    auto (*m_c_exists)(void* handle, uint64_t id) -> object_ptr<Bool>;
    /// info
    auto (*m_n_info)(void* handle, uint64_t id) -> object_ptr<NodeInfo>;
    auto (*m_s_info)(void* handle, uint64_t id) -> object_ptr<SocketInfo>;
    auto (*m_c_info)(void* handle, uint64_t id) -> object_ptr<ConnectionInfo>;
    /// set_name
    void (*m_n_set_name)(void* handle, uint64_t id);
    void (*m_s_set_name)(void* handle, uint64_t id);
  };

  /// Interface of node graph
  using NodeGraphInterface = Box<node_graph_interface_object_value>;

} // namespace yave