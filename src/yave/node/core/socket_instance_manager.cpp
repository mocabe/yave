//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/socket_instance_manager.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(socket_instance_manager)

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
    auto lck = other._lock();
  }

  socket_instance_manager::socket_instance_manager(
    socket_instance_manager&& other) noexcept
    : m_map {std::move(other.m_map)}
    , m_mtx {}
  {
    auto lck = other._lock();
  }

  socket_instance_manager& socket_instance_manager::operator=(
    const socket_instance_manager& other)
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    m_map = other.m_map;
    return *this;
  }

  socket_instance_manager& socket_instance_manager::operator=(
    socket_instance_manager&& other) noexcept
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    m_map = std::move(other.m_map);
    return *this;
  }

  auto socket_instance_manager::find(const socket_handle& socket) const
    -> std::optional<socket_instance>
  {
    auto lck = _lock();

    auto iter = m_map.find(socket.id());

    if (iter == m_map.end())
      return std::nullopt;

    return iter->second;
  }

  void socket_instance_manager::add(
    const socket_handle& socket,
    const socket_instance& socket_instance)
  {
    auto lck = _lock();

    auto iter = m_map.find(socket.id());

    if (iter != m_map.end()) {
      iter->second = socket_instance;
      return;
    }

    m_map.emplace(socket.id(), socket_instance);
  }

  void socket_instance_manager::remove(const socket_handle& socket)
  {
    auto lck = _lock();

    auto iter = m_map.find(socket.id());

    if (iter != m_map.end())
      m_map.erase(iter);
  }

  void socket_instance_manager::clear()
  {
    auto lck = _lock();
    m_map.clear();
  }

  bool socket_instance_manager::empty() const
  {
    auto lck = _lock();
    return m_map.empty();
  }

  size_t socket_instance_manager::size() const
  {
    auto lck = _lock();
    return m_map.size();
  }

  auto socket_instance_manager::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock(m_mtx);
  }

} // namespace yave