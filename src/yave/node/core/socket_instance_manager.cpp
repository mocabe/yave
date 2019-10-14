//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/socket_instance_manager.hpp>
#include <yave/support/log.hpp>

namespace {
  // logger
  std::shared_ptr<spdlog::logger> g_inst_mngr_logger;

  // init
  void init_inst_mngr_logger()
  {
    [[maybe_unused]] static auto init_logger = [] {
      g_inst_mngr_logger = yave::add_logger("socket_instance_manager");
      return 1;
    }();
  }
} // namespace

namespace yave {

  socket_instance_manager::socket_instance_manager()
    : m_map {}
    , m_mtx {}
  {
    init_inst_mngr_logger();
  }

  socket_instance_manager::socket_instance_manager(
    const socket_instance_manager& other)
    : m_map {other.m_map}
    , m_mtx {}
  {
    auto lck = other._lock();
  }

  socket_instance_manager::socket_instance_manager(
    socket_instance_manager&& other)
    : m_map {std::move(other.m_map)}
    , m_mtx {}
  {
    auto lck = other._lock();
  }

  socket_instance_manager& socket_instance_manager::
    operator=(const socket_instance_manager& other)
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    m_map = other.m_map;
    return *this;
  }

  socket_instance_manager& socket_instance_manager::
    operator=(socket_instance_manager&& other)
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    m_map = std::move(other.m_map);
    return *this;
  }

  auto socket_instance_manager::find(const uid& id, const std::string& socket)
    const -> std::optional<socket_instance>
  {
    auto lck = _lock();

    auto [bgn, end] = m_map.equal_range(id);
    for (auto iter = bgn; iter != end; ++iter) {
      if (iter->second.socket == socket)
        return iter->second.si;
    }
    return std::nullopt;
  }

  void socket_instance_manager::add(
    const uid& id,
    const std::string& socket,
    const socket_instance& socket_instance)
  {
    auto lck = _lock();

    auto [bgn, end] = m_map.equal_range(id);
    for (auto iter = bgn; iter != end; ++iter) {
      if (iter->second.socket == socket) {
        iter->second.si = socket_instance;
        return;
      }
    }
    m_map.emplace(id, instanceTable {socket, socket_instance});
  }

  void socket_instance_manager::remove(const uid& id, const std::string& socket)
  {
    auto lck = _lock();

    auto [bgn, end] = m_map.equal_range(id);

    auto iter = bgn;
    while (iter != end) {
      if (iter->second.socket == socket)
        m_map.erase(iter++);
      else
        ++iter;
    }
  }

  void socket_instance_manager::remove(const uid& id)
  {
    auto lck = _lock();

    auto [bgn, end] = m_map.equal_range(id);
    auto iter       = bgn;
    while (iter != end) m_map.erase(iter++);
  }

  void socket_instance_manager::clear()
  {
    auto lck = _lock();
    m_map.clear();
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