//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_info.hpp>

#include <stdexcept>
#include <range/v3/algorithm.hpp>

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
        ranges::sort(names);
        return ranges::unique(names) == names.end();
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
    node_type type)
    : m_name {std::move(name)}
    , m_input_sockets {std::move(input_sockets)}
    , m_output_sockets {std::move(output_sockets)}
    , m_type {type}
  {
    // check
    validate_node_info(m_name, m_input_sockets, m_output_sockets);
  }

  auto node_info::name() const -> const std::string&
  {
    return m_name;
  }

  void node_info::set_name(std::string name)
  {
    m_name = std::move(name);
  }

  auto node_info::input_sockets() const -> const std::vector<std::string>&
  {
    return m_input_sockets;
  }

  auto node_info::output_sockets() const -> const std::vector<std::string>&
  {
    return m_output_sockets;
  }

  void node_info::set_output_sockets(std::vector<std::string> sockets)
  {
    m_input_sockets = std::move(sockets);
    validate_node_info(m_name, m_input_sockets, m_output_sockets);
  }

  void node_info::set_input_sockets(std::vector<std::string> sockets)
  {
    m_output_sockets = std::move(sockets);
    validate_node_info(m_name, m_input_sockets, m_output_sockets);
  }

  bool node_info::is_normal() const
  {
    return m_type == node_type::normal;
  }

  bool node_info::is_primitive() const
  {
    return m_type == node_type::primitive;
  }

  bool node_info::is_interface() const
  {
    return m_type == node_type::interface;
  }

  auto node_info::type() const -> node_type
  {
    return m_type;
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