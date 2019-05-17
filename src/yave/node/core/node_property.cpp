//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_property.hpp>

namespace yave {

  node_property::node_property(const std::string& name, const primitive_t& prim)
    : m_name {name}
    , m_prim {prim}
  {
  }

  node_property::node_property(const std::string& name, std::monostate)
    : m_name {name}
    , m_prim {std::monostate {}}
  {
  }

  bool node_property::is_prim() const
  {
    return !std::holds_alternative<std::monostate>(m_prim);
  }

  const std::string& node_property::name() const
  {
    return m_name;
  }

  primitive_t node_property::prim() const
  {
    return std::get<primitive_t>(m_prim);
  }

  void node_property::set_prim(const primitive_t& prim)
  {
    m_prim = prim;
  }

  void node_property::ser_prim(std::monostate)
  {
    m_prim = std::monostate {};
  }
}