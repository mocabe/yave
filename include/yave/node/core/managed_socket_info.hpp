//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/socket_info.hpp>

namespace yave {

  /// socket_info for managed node graph
  class managed_socket_info
  {
  public:
    managed_socket_info()                           = delete;
    managed_socket_info(const managed_socket_info&) = default;
    managed_socket_info(managed_socket_info&&)      = default;
    managed_socket_info& operator=(const managed_socket_info&) = default;
    managed_socket_info& operator=(managed_socket_info&&) = default;

    managed_socket_info(std::string name, socket_type type, node_handle node);

    /// name
    [[nodiscard]] auto name() const -> const std::string&;

    /// socket type
    [[nodiscard]] auto type() const -> socket_type;

    /// Get node
    [[nodiscard]] auto node() const -> const node_handle&;

  private:
    std::string m_name;
    socket_type m_type;
    node_handle m_node;
  };

} // namespace yave