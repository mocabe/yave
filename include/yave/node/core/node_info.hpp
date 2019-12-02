//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_property.hpp>
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
      std::vector<std::string> input_sockets,
      std::vector<std::string> output_sockets,
      node_type = node_type::normal);

    /// name
    [[nodiscard]] auto name() const -> const std::string&;

    /// input sockets
    [[nodiscard]] auto input_sockets() const 
      -> const std::vector<std::string>&;

    /// output sockets
    [[nodiscard]] auto output_sockets() const
      -> const std::vector<std::string>&;

    /// normal?
    [[nodiscard]] bool is_normal() const;
    /// primitive?
    [[nodiscard]] bool is_primitive() const;
    /// interface?
    [[nodiscard]] bool is_interface() const;
    /// node type
    [[nodiscard]] auto type() const -> node_type;

    /// set name
    void set_name(std::string name);

    /// set input sockets
    void set_input_sockets(std::vector<std::string> sockets);

    /// set output sockets
    void set_output_sockets(std::vector<std::string> sockets);

  private:
    /// Unique name of node.
    std::string m_name;
    /// List of input sockets.
    std::vector<std::string> m_input_sockets;
    /// List of output sockets.
    std::vector<std::string> m_output_sockets;
    /// node type
    node_type m_type;
  };

} // namespace yave