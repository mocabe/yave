//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_info.hpp>

#include <stdexcept>
#include <algorithm>

namespace yave {

  namespace {
    void validate_node_info(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::vector<std::string>& output_sockets)
    {
      if (name == "")
        throw std::invalid_argument("Empty name of node_info is not allowed");

      for (auto&& is : input_sockets) {
        if (is == "")
          throw std::invalid_argument("Empty socket name is not allowed");
      }

      for (auto&& os : output_sockets) {
        if (os == "")
          throw std::invalid_argument("Empty socket name is not allowed");
      }

      auto _has_unique_names = [](auto names) {
        // get unique names
        std::sort(names.begin(), names.end());
        auto end = std::unique(names.begin(), names.end());
        return end == names.end();
      };

      if (
        !_has_unique_names(input_sockets) || !_has_unique_names(output_sockets))
        throw std::invalid_argument("Socket names should be unique");
    }
  } // namespace

  node_info::node_info(
    std::string name,
    std::vector<std::string> input_sockets,
    std::vector<std::string> output_sockets,
    bool is_prim)
    : m_name {std::move(name)}
    , m_input_sockets {std::move(input_sockets)}
    , m_output_sockets {std::move(output_sockets)}
    , m_is_prim {is_prim}
  {
    // check
    validate_node_info(m_name, m_input_sockets, m_output_sockets);
  }

  auto node_info::name() const -> const std::string&
  {
    return m_name;
  }

  void node_info::set_name(const std::string& name)
  {
    m_name = name;
  }

  auto node_info::input_sockets() const -> const std::vector<std::string>&
  {
    return m_input_sockets;
  }

  auto node_info::output_sockets() const -> const std::vector<std::string>&
  {
    return m_output_sockets;
  }

  void node_info::set_output_sockets(const std::vector<std::string>& sockets)
  {
    m_input_sockets = sockets;
    validate_node_info(m_name, m_input_sockets, m_output_sockets);
  }

  void node_info::set_input_sockets(const std::vector<std::string>& sockets)
  {
    m_output_sockets = sockets;
    validate_node_info(m_name, m_input_sockets, m_output_sockets);
  }

  bool node_info::is_prim() const
  {
    return m_is_prim;
  }

  bool operator==(const node_info& lhs, const node_info& rhs)
  {
    return lhs.name() == rhs.name() &&
           lhs.input_sockets() == rhs.input_sockets() &&
           lhs.output_sockets() == rhs.output_sockets();
  }

  bool operator!=(const node_info& lhs, const node_info& rhs)
  {
    return !(lhs == rhs);
  }

} // namespace yave