//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/socket_instance_manager.hpp>

namespace yave {

  SocketInstanceManager::SocketInstanceManager()
    : m_map {}
    , m_mtx {}
  {
  }

  SocketInstanceManager::SocketInstanceManager(
    const SocketInstanceManager& other)
    : m_map {other.m_map}
    , m_mtx {}
  {
  }

  SocketInstanceManager::SocketInstanceManager(SocketInstanceManager&& other)
    : m_map {std::move(other.m_map)}
    , m_mtx {}
  {
  }

  SocketInstanceManager& SocketInstanceManager::
    operator=(const SocketInstanceManager& other)
  {
    m_map = other.m_map;
    return *this;
  }

  SocketInstanceManager& SocketInstanceManager::
    operator=(SocketInstanceManager&& other)
  {
    m_map = std::move(other.m_map);
    return *this;
  }

  std::optional<SocketInstance> SocketInstanceManager::find(
    const NodeHandle& h,
    const std::string& socket) const
  {
    auto [bgn, end] = m_map.equal_range(h);
    for (auto iter = bgn; iter != end; ++iter) {
      if (iter->second.socket == socket)
        return iter->second.instance;
    }
    return std::nullopt;
  }

  void SocketInstanceManager::add(
    const NodeHandle& h,
    const std::string& socket,
    const SocketInstance& instance)
  {
    auto [bgn, end] = m_map.equal_range(h);
    for (auto iter = bgn; iter != end; ++iter) {
      if (iter->second.socket == socket) {
        iter->second.instance = instance;
        return;
      }
    }
    m_map.emplace(h, instanceTable {socket, instance});
  }

  void SocketInstanceManager::remove(
    const NodeHandle& h,
    const std::string& socket)
  {
    auto [bgn, end] = m_map.equal_range(h);

    auto iter = bgn;
    while (iter != end) {
      if (iter->second.socket == socket)
        m_map.erase(iter++);
      else
        ++iter;
    }
  }

  void SocketInstanceManager::remove(const NodeHandle& h)
  {
    auto [bgn, end] = m_map.equal_range(h);
    auto iter       = bgn;
    while (iter != end) m_map.erase(iter++);
  }

  [[nodiscard]] std::unique_lock<std::mutex> SocketInstanceManager::lock() const
  {
    return std::unique_lock(m_mtx);
  }

  void SocketInstanceManager::clear()
  {
    m_map.clear();
  }

  size_t SocketInstanceManager::size() const
  {
    return m_map.size();
  }
} // namespace yave