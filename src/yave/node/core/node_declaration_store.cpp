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
    auto [it, succ] = m_map.emplace(
      decl.qualified_name(), std::make_shared<node_declaration>(decl));

    if (succ)
      Info(g_logger, "Added new declaration: {}", decl.qualified_name());
    else
      Error(g_logger, "Failed to add declaration: {}", decl.qualified_name());

    return succ;
  }

  bool node_declaration_store::add(const std::vector<node_declaration>& decls)
  {
    std::vector<std::string> added;

    for (auto&& decl : decls) {
      if (add(decl)) {
        added.push_back(decl.qualified_name());
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
    -> std::shared_ptr<const node_declaration>
  {
    auto iter = m_map.find(name);

    if (iter == m_map.end())
      return nullptr;

    return iter->second;
  }

  auto node_declaration_store::size() const -> size_t
  {
    return m_map.size();
  }

  auto node_declaration_store::enumerate() const
    -> std::vector<std::shared_ptr<node_declaration>>
  {
    std::vector<std::shared_ptr<node_declaration>> ret;
    for (auto&& [key, decl] : m_map) {
      (void)key;
      ret.push_back(decl);
    }
    return ret;
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