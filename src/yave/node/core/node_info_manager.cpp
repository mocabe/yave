//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_info_manager.hpp>
#include <yave/support/log.hpp>

#include <cassert>

namespace {
  // logger
  std::shared_ptr<spdlog::logger> g_info_mngr_logger;

  // init
  void init_info_mngr_logger()
  {
    [[maybe_unused]] static auto init_logger = [] {
      g_info_mngr_logger = yave::add_logger("node_info_manager");
      return 1;
    }();
  }
} // namespace

namespace yave {

  node_info_manager::node_info_manager()
    : m_info {}
    , m_mtx {}
  {
    init_info_mngr_logger();
  }

  node_info_manager::node_info_manager(const std::vector<node_info>& info_list)
  {
    init_info_mngr_logger();

    for (auto&& i : info_list) {
      if (!add(i))
        throw std::runtime_error("Failed to add node_info on initialization");
    }
  }

  node_info_manager::node_info_manager(const node_info_manager& other)
    : m_info {other.m_info}
    , m_mtx {}
  {
    auto lck = other._lock();
  }

  node_info_manager::node_info_manager(node_info_manager&& other)
    : m_info {std::move(other.m_info)}
    , m_mtx {}
  {
    auto lck = other._lock();
  }

  node_info_manager& node_info_manager::
    operator=(const node_info_manager& other)
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();
    
    m_info = other.m_info;
    return *this;
  }

  node_info_manager& node_info_manager::operator=(node_info_manager&& other)
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    m_info = std::move(other.m_info);
    return *this;
  }

  bool node_info_manager::add(const info_type& info)
  {
    auto lck = _lock();

    auto [it, succ] =
      m_info.emplace(info.name(), std::make_shared<info_type>(info));

    if (succ) {
      Info(
        g_info_mngr_logger,
        "Added new node_info: {}(in:{},out:{})",
        it->first,
        it->second->input_sockets().size(),
        it->second->output_sockets().size());
      return true;
    }

    // when exact same info is already there
    if (*it->second == info) {
      assert(it->first == info.name());
      Warning(
        g_info_mngr_logger,
        "Tried to add same node_info multiple times: {}",
        info.name());
      return true;
    }

    Warning(
      g_info_mngr_logger,
      "Tried to add node_info which is different from exising one: {}",
      it->second->name());

    return false;
  }

  void node_info_manager::remove(const std::string& name)
  {
    auto lck = _lock();

    // find
    auto it = m_info.find(name);
    // not found
    if (it == m_info.end())
      return;
    // remove
    m_info.erase(it);

    Info(g_info_mngr_logger, "Removed node_info: {}", name);
  }

  void node_info_manager::remove(const node_info& info)
  {
    auto lck = _lock();

    auto iter = m_info.begin();
    auto end  = m_info.end();
    while (iter != end) {
      if (*iter->second == info) {
        Info(g_info_mngr_logger, "Removed node_info: {}", iter->second->name());
        m_info.erase(iter++);
      } else
        ++iter;
    }
  }

  bool node_info_manager::exists(const std::string& name) const
  {
    auto lck = _lock();
    return m_info.find(name) != m_info.end();
  }

  auto node_info_manager::enumerate()
    -> std::vector<std::shared_ptr<const info_type>>
  {
    auto lck = _lock();

    std::vector<std::shared_ptr<const info_type>> ret;
    for (auto&& [name, ptr] : m_info) {
      assert(name == ptr->name());
      ret.push_back(ptr);
    }
    return ret;
  }

  auto node_info_manager::find(const std::string& name) const
    -> std::shared_ptr<const info_type>
  {
    auto lck = _lock();

    auto iter = m_info.find(name);
    if (iter == m_info.end())
      return nullptr;
    return iter->second;
  }

  void node_info_manager::clear()
  {
    auto lck = _lock();
    m_info.clear();
  }

  auto node_info_manager::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock(m_mtx);
  }

} // namespace yave