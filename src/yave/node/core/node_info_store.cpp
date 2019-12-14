//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//


#include <yave/node/core/node_info_store.hpp>

#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(node_info_store)

namespace yave {

  node_info_store::node_info_store()
  {
    init_logger();
  }

  node_info_store::node_info_store(const node_info_store& other)
    : m_map {other.m_map}
  {
  }

  node_info_store::node_info_store(node_info_store&& other) noexcept
    : m_map {std::move(other.m_map)}
  {
  }

  node_info_store& node_info_store::operator=(const node_info_store& other)
  {
    m_map = other.m_map;
    return *this;
  }

  node_info_store& node_info_store::operator=(node_info_store&& other) noexcept
  {
    m_map = std::move(other.m_map);
    return *this;
  }

  bool node_info_store::add(const node_info& info)
  {
    auto [it, succ] =
      m_map.emplace(info.name(), std::make_shared<node_info>(info));

    if (!succ)
      Error(g_logger, "Failed to add node info: {}", info.name());

    return succ;
  }

  bool node_info_store::add(const std::vector<node_info>& info)
  {
    std::vector<std::string> added;

    for (auto&& i : info) {
      if (add(i)) {
        added.push_back(i.name());
      } else {
        for (auto&& n : added)
          remove(n);
        return false;
      }
    }
    return true;
  }

  bool node_info_store::exists(const std::string& name) const
  {
    return m_map.find(name) != m_map.end();
  }

  auto node_info_store::find(const std::string& name) const
    -> std::shared_ptr<const node_info>
  {
    auto it = m_map.find(name);

    if (it == m_map.end())
      return nullptr;

    return it->second;
  }

  void node_info_store::remove(const std::string& name)
  {
    auto iter = m_map.find(name);

    if (iter == m_map.end())
      return;

    m_map.erase(iter);
  }

  void node_info_store::remove(const std::vector<std::string>& names)
  {
    for (auto&& name : names)
      remove(name);
  }

  void node_info_store::clear()
  {
    m_map.clear();
  }

  bool node_info_store::empty() const
  {
    return m_map.empty();
  }
} // namespace yave