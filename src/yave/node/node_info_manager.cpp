//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/node_info_manager.hpp>

namespace yave {

  NodeInfoManager::NodeInfoManager(const NodeInfoManager& other)
    : m_info {other.m_info}
    , m_mtx {}
  {
  }

  NodeInfoManager::NodeInfoManager(NodeInfoManager&& other)
    : m_info {std::move(other.m_info)}
    , m_mtx {}
  {
  }

  NodeInfoManager& NodeInfoManager::operator=(const NodeInfoManager& other)
  {
    m_info = other.m_info;
    return *this;
  }

  NodeInfoManager& NodeInfoManager::operator=(NodeInfoManager&& other)
  {
    m_info = std::move(other.m_info);
    return *this;
  }

  NodeInfoManager::NodeInfoManager()
    : m_info {}
    , m_mtx {}
  {
  }

  [[nodiscard]] bool NodeInfoManager::add(const info_type& info)
  {

    auto succ = m_info.emplace(info.name(), info).second;
    return succ;
  }

  void NodeInfoManager::remove_iterator(map_type::const_iterator iter)
  {
    // erase
    m_info.erase(iter);
  }

  void NodeInfoManager::remove(const std::string& name)
  {
    // find
    auto it = m_info.find(name);
    // not found
    if (it == m_info.end())
      return;
    // remove
    remove_iterator(it);
  }

  void NodeInfoManager::remove(map_type::const_iterator iter)
  {
    // validate iterator
    iter = [&]() {
      for (auto it = m_info.cbegin(); it != m_info.cend(); ++it) {
        if (iter == it)
          return iter;
      }
      return m_info.cend();
    }();
    if (iter == m_info.end())
      return;
    // remove
    remove_iterator(iter);
  }

  void NodeInfoManager::remove(const NodeInfo& info)
  {
    auto iter = m_info.begin();
    auto end  = m_info.end();
    while (iter != end) {
      if (iter->second == info) {
        m_info.erase(iter++);
      }
    }
  }

  [[nodiscard]] bool NodeInfoManager::exists(const std::string& name) const
  {
    return m_info.find(name) != m_info.end();
  };

  [[nodiscard]] const NodeInfoManager::info_type*
    NodeInfoManager::find(const std::string& name) const
  {
    auto iter = m_info.find(name);
    if (iter == m_info.end())
      return nullptr;
    return &iter->second;
  };

  [[nodiscard]] NodeInfoManager::const_iterator NodeInfoManager::cbegin() const
  {
    return m_info.cbegin();
  }

  [[nodiscard]] NodeInfoManager::const_iterator NodeInfoManager::cend() const
  {
    return m_info.cend();
  }

  [[nodiscard]] std::unique_lock<std::mutex> NodeInfoManager::lock() const
  {
    return std::unique_lock(m_mtx);
  }

  void NodeInfoManager::clear()
  {
    m_info.clear();
  }

} // namespace yave