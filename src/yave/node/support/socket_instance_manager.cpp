//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/socket_instance_manager.hpp>

namespace yave {

  socket_instance_manager::socket_instance_manager()
    : m_map {}
    , m_mtx {}
  {
  }

  socket_instance_manager::socket_instance_manager(
    const socket_instance_manager& other)
    : m_map {other.m_map}
    , m_mtx {}
  {
  }

  socket_instance_manager::socket_instance_manager(
    socket_instance_manager&& other)
    : m_map {std::move(other.m_map)}
    , m_mtx {}
  {
  }

  socket_instance_manager& socket_instance_manager::
    operator=(const socket_instance_manager& other)
  {
    m_map = other.m_map;
    return *this;
  }

  socket_instance_manager& socket_instance_manager::
    operator=(socket_instance_manager&& other)
  {
    m_map = std::move(other.m_map);
    return *this;
  }

  std::optional<socket_instance> socket_instance_manager::find(
    const node_handle& h,
    const std::string& socket) const
  {
    auto [bgn, end] = m_map.equal_range(h);
    for (auto iter = bgn; iter != end; ++iter) {
      if (iter->second.socket == socket)
        return iter->second.instance;
    }
    return std::nullopt;
  }

  void socket_instance_manager::add(
    const node_handle& h,
    const std::string& socket,
    const socket_instance& instance)
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

  void socket_instance_manager::remove(
    const node_handle& h,
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

  void socket_instance_manager::remove(const node_handle& h)
  {
    auto [bgn, end] = m_map.equal_range(h);
    auto iter       = bgn;
    while (iter != end) m_map.erase(iter++);
  }

  [[nodiscard]] std::unique_lock<std::mutex>
    socket_instance_manager::lock() const
  {
    return std::unique_lock(m_mtx);
  }

  void socket_instance_manager::clear()
  {
    m_map.clear();
  }

  size_t socket_instance_manager::size() const
  {
    return m_map.size();
  }
} // namespace yave