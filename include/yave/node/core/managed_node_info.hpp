//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_info.hpp>

namespace yave {

  enum class managed_node_type
  {
    normal,
    primitive,
    group,
    group_input,
    group_output,
  };

  /// node_info for managed node graph
  class managed_node_info
  {
  public:
    managed_node_info()                         = delete;
    managed_node_info(const managed_node_info&) = default;
    managed_node_info(managed_node_info&&)      = default;
    managed_node_info& operator=(const managed_node_info&) = default;
    managed_node_info& operator=(managed_node_info&&) = default;

    managed_node_info(
      std::string name,
      std::vector<std::string> input_sockets,
      std::vector<std::string> output_sockets,
      managed_node_type type);

    /// name
    [[nodiscard]] auto name() const -> const std::string&;

    /// input sockets
    [[nodiscard]] auto input_sockets() const //
      -> const std::vector<std::string>&;

    /// output sockets
    [[nodiscard]] auto output_sockets() const //
      -> const std::vector<std::string>&;

    /// normal?
    [[nodiscard]] bool is_normal() const;
    /// primitive?
    [[nodiscard]] bool is_primitive() const;
    /// group?
    [[nodiscard]] bool is_group() const;
    /// group input?
    [[nodiscard]] bool is_group_input() const;
    /// group output?
    [[nodiscard]] bool is_group_output() const;
    /// type
    [[nodiscard]] auto type() const -> managed_node_type;

  private:
    /// Unique name of node.
    std::string m_name;
    /// List of input sockets.
    std::vector<std::string> m_input_sockets;
    /// List of output sockets.
    std::vector<std::string> m_output_sockets;
    /// node type
    managed_node_type m_type;
  };
}