//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_property.hpp>

namespace yave {

  node_property::node_property(const std::string& name, const primitive_t& prim)
    : m_name {name}
    , m_prim {std::make_shared<primitive_container>(prim)}
  {
  }

  node_property::node_property(const std::string& name, std::monostate)
    : m_name {name}
    , m_prim {std::nullopt}
  {
  }

  bool node_property::is_prim() const
  {
    return m_prim.has_value();
  }

  const std::string& node_property::name() const
  {
    return m_name;
  }

  std::optional<primitive_t> node_property::get_prim() const
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

  std::shared_ptr<primitive_container> node_property::get_shared_prim() const
  {
    if (is_prim())
      return m_prim.value();
    else
      return nullptr;
  }
} // namespace yave