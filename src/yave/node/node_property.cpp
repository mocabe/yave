//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/node_property.hpp>

namespace yave {

  NodeProperty::NodeProperty(const std::string& name, const primitive_t& prim)
    : m_name {name}
    , m_prim {prim}
  {
  }

  NodeProperty::NodeProperty(const std::string& name, std::monostate)
    : m_name {name}
    , m_prim {std::monostate {}}
  {
  }

  bool NodeProperty::is_prim() const
  {
    return !std::holds_alternative<std::monostate>(m_prim);
  }

  std::string NodeProperty::name() const
  {
    return m_name;
  }

  primitive_t NodeProperty::prim() const
  {
    return std::get<primitive_t>(m_prim);
  }

  void NodeProperty::set_prim(const primitive_t& prim)
  {
    m_prim = prim;
  }

  void NodeProperty::ser_prim(std::monostate)
  {
    m_prim = std::monostate {};
  }
}