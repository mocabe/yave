//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_property.hpp>
#include <yave/node/core/socket_handle.hpp>
#include <string>
#include <vector>

namespace yave {

  /// node_info represents node interface for frontend.
  /// Each node can have multiple input sockets and output sockets.
  /// Sockets are represented by string name, and you cannot have duplicated
  /// socket name for each of both input/output sockets.
  class node_info
  {
  public:
    node_info()                 = delete;
    node_info(const node_info&) = default;
    node_info(node_info&&)      = default;
    node_info& operator=(const node_info&) = default;
    node_info& operator=(node_info&&) = default;

    node_info(
      std::string name,
      std::vector<socket_handle> input_sockets,
      std::vector<socket_handle> output_sockets,
      node_type = node_type::normal);

    /// name
    [[nodiscard]] auto name() const -> const std::string&;

    /// type
    [[nodiscard]] auto type() const -> node_type;

    /// get sockets
    [[nodiscard]] auto sockets(socket_type) const
      -> const std::vector<socket_handle>&;

    /// set name
    void set_name(std::string name);

  private:
    /// Unique name of node.
    std::string m_name;
    /// List of input sockets.
    std::vector<socket_handle> m_input_sockets;
    /// List of output sockets.
    std::vector<socket_handle> m_output_sockets;
    /// node type
    node_type m_type;
  };

} // namespace yave