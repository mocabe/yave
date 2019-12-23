//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_property.hpp>
#include <yave/rts/utility.hpp>

namespace yave {

  node_property::node_property(const std::string& name, node_type type)
    : m_name {name}
    , m_type {type}
    , m_visited {false}
  {
  }

  auto node_property::name() const -> const std::string&
  {
    return m_name;
  }

  bool node_property::is_normal() const
  {
    return m_type == node_type::normal;
  }

  bool node_property::is_interface() const
  {
    return m_type == node_type::interface;
  }

  node_type node_property::type() const
  {
    return m_type;
  }

  void node_property::set_name(const std::string& new_name)
  {
    m_name = new_name;
  }

  bool node_property::is_visited() const
  {
    return m_visited;
  }

  bool node_property::is_unvisited() const
  {
    return !m_visited;
  }

  void node_property::set_visited() const
  {
    m_visited = true;
  }

  void node_property::set_unvisited() const
  {
    m_visited = false;
  }
} // namespace yave