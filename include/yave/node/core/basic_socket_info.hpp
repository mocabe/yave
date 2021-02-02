//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/basic_socket_property.hpp>

namespace yave {

  class basic_socket_info
  {
  public:
    basic_socket_info()                         = delete;
    basic_socket_info(const basic_socket_info&) = default;
    basic_socket_info(basic_socket_info&&)      = default;
    basic_socket_info& operator=(const basic_socket_info&) = default;
    basic_socket_info& operator=(basic_socket_info&&) = default;

    basic_socket_info(
      std::string name,
      basic_socket_type type,
      node_handle node,
      std::vector<node_handle> interfaces);

    /// name
    [[nodiscard]] auto name() const -> const std::string&;

    /// socket type
    [[nodiscard]] auto type() const -> basic_socket_type;

    /// Get node
    [[nodiscard]] auto node() const -> const node_handle&;

    /// Get interfaces
    [[nodiscard]] auto interfaces() const -> const std::vector<node_handle>&;

  private:
    std::string m_name;
    basic_socket_type m_type;
    node_handle m_node;
    std::vector<node_handle> m_interfaces;
  };
} // namespace yave