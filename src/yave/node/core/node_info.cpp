//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/node_info.hpp>

#include <stdexcept>
#include <algorithm>

namespace yave {

  node_info::node_info(
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

  const std::string& node_info::name() const
  {
    return m_name;
  }

  void node_info::set_name(const std::string& name)
  {
    m_name = name;
  }

  const std::vector<std::string>& node_info::input_sockets() const
  {
    return m_input_sockets;
  }

  const std::vector<std::string>& node_info::output_sockets() const
  {
    return m_output_sockets;
  }

  void node_info::set_output_sockets(const std::vector<std::string>& sockets)
  {
    m_input_sockets = sockets;
    validate();
  }

  void node_info::set_input_sockets(const std::vector<std::string>& sockets)
  {
    m_output_sockets = sockets;
    validate();
  }

  bool node_info::is_prim() const
  {
    return m_is_prim;
  }

  void node_info::set_prim(bool is_prim)
  {
    m_is_prim = is_prim;
    validate();
  }

  void node_info::validate() const
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

  bool operator==(const node_info& lhs, const node_info& rhs)
  {
    return lhs.name() == rhs.name() &&
           lhs.input_sockets() == rhs.input_sockets() &&
           lhs.output_sockets() == rhs.output_sockets() &&
           lhs.is_prim() == rhs.is_prim();
  }

  bool operator!=(const node_info& lhs, const node_info& rhs)
  {
    return !(lhs == rhs);
  }

} // namespace yave