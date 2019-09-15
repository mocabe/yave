//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_property.hpp>

namespace yave {

  node_property::node_property(const std::string& name, const primitive_t& prim)
    : m_name {name}
    , m_prim {make_object<PrimitiveContainer>(prim)}
    , m_visited {0}
  {
  }

  node_property::node_property(const std::string& name, std::monostate)
    : m_name {name}
    , m_prim {std::nullopt}
    , m_visited {0}
  {
  }

  bool node_property::is_prim() const
  {
    return m_prim.has_value();
  }

  auto node_property::name() const -> const std::string&
  {
    return m_name;
  }

  auto node_property::get_prim() const -> std::optional<primitive_t>
  {
    if (is_prim())
      return m_prim.value()->get();
    else
      return std::nullopt;
  }

  void node_property::set_prim(const primitive_t& prim)
  {
    if (is_prim())
      m_prim.value()->set(prim);
  }

  void node_property::ser_prim(std::nullopt_t)
  {
    m_prim = std::nullopt;
  }

  auto node_property::get_shared_prim() const -> object_ptr<PrimitiveContainer>
  {
    if (is_prim())
      return m_prim.value();
    else
      return nullptr;
  }
} // namespace yave