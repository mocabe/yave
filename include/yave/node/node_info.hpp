//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <string>
#include <vector>

namespace yave {

  /// Node info
  class NodeInfo
  {
  public:
    NodeInfo()                = delete;
    NodeInfo(const NodeInfo&) = default;
    NodeInfo(NodeInfo&&)      = default;
    NodeInfo& operator=(const NodeInfo&) = default;
    NodeInfo& operator=(NodeInfo&&) = default;
    NodeInfo(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::vector<std::string>& output_sockets,
      bool is_prim = false);

    /// name
    [[nodiscard]] const std::string& name() const;

    /// set name
    void set_name(const std::string& name);

    /// input sockets
    [[nodiscard]] const std::vector<std::string>& input_sockets() const;

    /// set input sockets
    void set_input_sockets(const std::vector<std::string>& sockets);

    /// output sockets
    [[nodiscard]] const std::vector<std::string>& output_sockets() const;

    /// set output sockets
    void set_output_sockets(const std::vector<std::string>& sockets);

    /// is_prim
    [[nodiscard]] bool is_prim() const;

    /// set prim
    void set_prim(bool is_prim);

  private:
    void validate() const;

  private:
    /// name of node
    std::string m_name;
    /// list of input sockets
    std::vector<std::string> m_input_sockets;
    /// list of output sockets
    std::vector<std::string> m_output_sockets;
    /// primitive node flag
    bool m_is_prim;
  };

  /// operator==
  [[nodiscard]] bool operator==(const NodeInfo& lhs, const NodeInfo& rhs);

  /// operator!=
  [[nodiscard]] bool operator!=(const NodeInfo& lhs, const NodeInfo& rhs);

} // namespace yave