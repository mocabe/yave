//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/node_info.hpp>

#include <stdexcept>
#include <algorithm>

namespace yave {

  NodeInfo::NodeInfo(
    const std::string& name,
    const std::vector<std::string>& input_sockets,
    const std::vector<std::string>& output_sockets,
    bool is_prim)
    : m_name {name}
    , m_input_sockets {input_sockets}
    , m_output_sockets {output_sockets}
    , m_is_prim {is_prim}
  {
    // check
    validate();
  }

  const std::string& NodeInfo::name() const
  {
    return m_name;
  }

  void NodeInfo::set_name(const std::string& name)
  {
    m_name = name;
  }

  const std::vector<std::string>& NodeInfo::input_sockets() const
  {
    return m_input_sockets;
  }

  const std::vector<std::string>& NodeInfo::output_sockets() const
  {
    return m_output_sockets;
  }

  void NodeInfo::set_output_sockets(const std::vector<std::string>& sockets)
  {
    m_input_sockets = sockets;
    validate();
  }

  void NodeInfo::set_input_sockets(const std::vector<std::string>& sockets)
  {
    m_output_sockets = sockets;
    validate();
  }

  bool NodeInfo::is_prim() const
  {
    return m_is_prim;
  }

  void NodeInfo::set_prim(bool is_prim)
  {
    m_is_prim = is_prim;
    validate();
  }

  void NodeInfo::validate() const
  {
    auto _has_unique_names = [](auto names) {
      // get unique names
      std::sort(names.begin(), names.end());
      auto end = std::unique(names.begin(), names.end());
      return end == names.end();
    };
    if (m_is_prim) {
      if (!m_input_sockets.empty())
        throw std::invalid_argument("Primitive node cannot have input sockets");
      if (m_output_sockets.size() != 1)
        throw std::invalid_argument(
          "Primitive node should have 1 output socket");
    } else {
      if (m_output_sockets.empty())
        throw std::invalid_argument("Node should have output sockets");
    }
    if (
      !_has_unique_names(m_input_sockets) ||
      !_has_unique_names(m_output_sockets))
      throw std::invalid_argument("Socket names should be unique");
  }

  bool operator==(const NodeInfo& lhs, const NodeInfo& rhs)
  {
    return lhs.name() == rhs.name() &&
           lhs.input_sockets() == rhs.input_sockets() &&
           lhs.output_sockets() == rhs.output_sockets() &&
           lhs.is_prim() == rhs.is_prim();
  }

} // namespace yave