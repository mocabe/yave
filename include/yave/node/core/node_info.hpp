//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

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
      bool is_prim = false);

    /// name
    [[nodiscard]] auto name() const -> const std::string&;

    /// set name
    void set_name(const std::string& name);

    /// input sockets
    [[nodiscard]] auto input_sockets() const -> const std::vector<std::string>&;

    /// set input sockets
    void set_input_sockets(const std::vector<std::string>& sockets);

    /// output sockets
    [[nodiscard]] auto output_sockets() const
      -> const std::vector<std::string>&;

    /// set output sockets
    void set_output_sockets(const std::vector<std::string>& sockets);

    /// primitive?
    [[nodiscard]] bool is_prim() const;

  private:
    /// Unique name of node.
    std::string m_name;
    /// List of input sockets.
    std::vector<std::string> m_input_sockets;
    /// List of output sockets.
    std::vector<std::string> m_output_sockets;
    /// enable primitive value for this node
    bool m_is_prim;
  };

  /// operator==
  [[nodiscard]] bool operator==(const node_info& lhs, const node_info& rhs);

  /// operator!=
  [[nodiscard]] bool operator!=(const node_info& lhs, const node_info& rhs);

} // namespace yave