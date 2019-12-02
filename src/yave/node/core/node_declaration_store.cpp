//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_declaration_store.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(node_declaration_store)

namespace yave {

  node_declaration_store::node_declaration_store()
    : m_map {}
  {
    init_logger();
  }

  node_declaration_store::node_declaration_store(
    const node_declaration_store& other)
    : m_map {other.m_map}
  {
  }

  node_declaration_store::node_declaration_store(
    node_declaration_store&& other) noexcept
    : m_map {std::move(other.m_map)}
  {
  }

  node_declaration_store& node_declaration_store::operator=(
    const node_declaration_store& other)
  {
    m_map = other.m_map;
    return *this;
  }

  node_declaration_store& node_declaration_store::operator=(
    node_declaration_store&& other) noexcept
  {
    m_map = std::move(other.m_map);
    return *this;
  }

  bool node_declaration_store::add(const node_declaration& decl)
  {
    assert(decl.node_type() != node_type::interface);

    auto [it, succ] =
      m_map.emplace(decl.name(), std::make_shared<node_declaration>(decl));

    if (succ)
      Info(g_logger, "Added new declaration: {}", decl.name());
    else
      Error(g_logger, "Failed to add declaration: {}", decl.name());

    return succ;
  }

  bool node_declaration_store::add(const std::vector<node_declaration>& decls)
  {
    std::vector<std::string> added;

    for (auto&& decl : decls) {
      assert(decl.node_type() != node_type::interface);
      if (add(decl)) {
        added.push_back(decl.name());
      } else {
        for (auto&& name : added) {
          remove(name);
        }
        return false;
      }
    }
    return true;
  }

  bool node_declaration_store::exists(const std::string& name) const
  {
    auto iter = m_map.find(name);
    return iter != m_map.end();
  }

  auto node_declaration_store::find(const std::string& name) const
    -> std::shared_ptr<node_declaration>
  {
    auto iter = m_map.find(name);

    if (iter != m_map.end())
      return iter->second;

    return nullptr;
  }

  void node_declaration_store::remove(const std::string& name)
  {
    auto iter = m_map.find(name);

    if (iter == m_map.end())
      return;

    Info(g_logger, "Removed declaration: {}", name);

    m_map.erase(iter);
  }

  void node_declaration_store::remove(const std::vector<std::string>& names)
  {
    for (auto&& name : names)
      remove(name);
  }

  void node_declaration_store::clear()
  {
    m_map.clear();
  }

  bool node_declaration_store::empty() const
  {
    return m_map.empty();
  }
}