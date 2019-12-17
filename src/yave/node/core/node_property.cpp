//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_property.hpp>
#include <yave/rts/utility.hpp>

namespace yave {

  node_property::node_property(normal_construct_t, const std::string& name)
    : m_name {name}
    , m_type {node_type::normal}
    , m_prim {std::nullopt}
    , m_visited {0}
  {
  }

  node_property::node_property(
    primitive_construct_t,
    const std::string& name,
    const primitive_t& prim)
    : m_name {name}
    , m_type {node_type::primitive}
    , m_prim {make_object<PrimitiveContainer>(prim)}
    , m_visited {0}
  {
  }

  node_property::node_property(interface_construct_t, const std::string& name)
    : m_name {name}
    , m_type {node_type::interface}
    , m_prim {std::nullopt}
    , m_visited {0}
  {
  }

  node_property::node_property(const node_property& other)
    : m_name {other.m_name}
    , m_type {other.m_type}
    , m_prim {other.m_prim}
    , m_visited {0}
  {
    // clone prim container
    if (m_prim)
      *m_prim = clone(*m_prim);
  }

  bool node_property::is_normal() const
  {
    return m_type == node_type::normal;
  }

  bool node_property::is_primitive() const
  {
    return m_type == node_type::primitive;
  }

  bool node_property::is_interface() const
  {
    return m_type == node_type::interface;
  }

  node_type node_property::type() const
  {
    return m_type;
  }

  auto node_property::name() const -> const std::string&
  {
    return m_name;
  }

  void node_property::set_name(const std::string& new_name)
  {
    m_name = new_name;
  }

  auto node_property::get_primitive() const -> primitive_t
  {
    assert(is_primitive());
    return m_prim.value()->get();
  }

  void node_property::set_primitive(const primitive_t& prim)
  {
    if (is_primitive())
      m_prim.value()->set(prim);
  }

  auto node_property::get_shared_primitive() const
    -> object_ptr<PrimitiveContainer>
  {
    if (is_primitive())
      return m_prim.value();
    else
      return nullptr;
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