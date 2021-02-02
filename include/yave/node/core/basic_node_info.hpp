//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/socket_handle.hpp>
#include <yave/node/core/basic_node_property.hpp>
#include <yave/node/core/basic_socket_property.hpp>

#include <string>
#include <vector>

namespace yave {

  /// node_info represents node interface for frontend.
  /// Each node can have multiple input sockets and output sockets.
  /// Sockets are represented by string name, and you cannot have duplicated
  /// socket name for each of both input/output sockets.
  class basic_node_info
  {
  public:
    basic_node_info()                       = delete;
    basic_node_info(const basic_node_info&) = default;
    basic_node_info(basic_node_info&&)      = default;
    basic_node_info& operator=(const basic_node_info&) = default;
    basic_node_info& operator=(basic_node_info&&) = default;

    basic_node_info(
      std::string name,
      std::vector<socket_handle> input_sockets,
      std::vector<socket_handle> output_sockets,
      basic_node_type = basic_node_type::normal);

    /// name
    [[nodiscard]] auto name() const -> const std::string&;

    /// type
    [[nodiscard]] auto type() const -> basic_node_type;

    /// get sockets
    [[nodiscard]] auto sockets(basic_socket_type) const
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
    basic_node_type m_type;
  };

} // namespace yave