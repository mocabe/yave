//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_info_manager.hpp>
#include <yave/support/log.hpp>

#include <cassert>

namespace {
  // logger
  std::shared_ptr<spdlog::logger> g_logger;

  // init
  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("node_info_manager");
      return 1;
    }();
  }
} // namespace

namespace yave {

  node_info_manager::node_info_manager()
    : m_map {}
    , m_mtx {}
  {
    init_logger();
  }

  node_info_manager::node_info_manager(const std::vector<node_info>& info_list)
  {
    init_logger();

    for (auto&& i : info_list) {
      if (!add(i))
        throw std::runtime_error("Failed to add node_info on initialization");
    }
  }

  node_info_manager::node_info_manager(const node_info_manager& other)
    : m_map {other.m_map}
    , m_mtx {}
  {
    auto lck = other._lock();
  }

  node_info_manager::node_info_manager(node_info_manager&& other)
    : m_map {std::move(other.m_map)}
    , m_mtx {}
  {
    auto lck = other._lock();
  }

  node_info_manager& node_info_manager::
    operator=(const node_info_manager& other)
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    m_map = other.m_map;
    return *this;
  }

  node_info_manager& node_info_manager::operator=(node_info_manager&& other)
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    m_map = std::move(other.m_map);
    return *this;
  }

  bool node_info_manager::add(const info_type& info)
  {
    auto lck = _lock();

    auto [it, succ] =
      m_map.emplace(info.name(), std::make_shared<info_type>(info));

    if (succ) {
      Info(
        g_logger,
        "Added new node_info: {}(in:{},out:{})",
        it->first,
        _access_info(it)->input_sockets().size(),
        _access_info(it)->output_sockets().size());
      return true;
    }

    // when exact same info is already there
    if (*_access_info(it) == info) {
      assert(it->first == info.name());
      Warning(
        g_logger,
        "Tried to add same node_info multiple times: {}",
        info.name());
      return true;
    }

    Error(
      g_logger,
      "Tried to add node_info which is different from exising one: {}",
      _access_info(it)->name());

    return false;
  }

  bool node_info_manager::add(const node_initializer& initializer)
  {
    auto lck = _lock();

    auto [it, succ] = m_map.emplace(
      initializer.name(),
      std::make_shared<const node_initializer>(initializer));

    if (succ) {
      Info(g_logger, "Added new node_initializer: {}", it->first);
      return true;
    }

    Error(
      g_logger,
      "Tried to add node_initializer which has same name to existing one");

    return false;
  }

  void node_info_manager::remove(const std::string& name)
  {
    auto lck = _lock();

    // find
    auto it = m_map.find(name);
    // not found
    if (it == m_map.end())
      return;
    // remove
    m_map.erase(it);

    Info(g_logger, "Removed node_info: {}", name);
  }

  void node_info_manager::remove(const node_info& info)
  {
    auto lck = _lock();

    auto iter = m_map.begin();
    auto end  = m_map.end();

    while (iter != end) {

      if (!std::get_if<std::shared_ptr<const node_info>>(&iter->second))
        continue;

      if (*_access_info(iter) == info) {
        Info(g_logger, "Removed node_info: {}", _access_info(iter)->name());
        m_map.erase(iter++);
      } else
        ++iter;
    }
  }

  void node_info_manager::remove(const node_initializer& initializer)
  {
    auto lck = _lock();

    auto iter = m_map.begin();
    auto end  = m_map.end();

    while (iter != end) {

      if (!std::get_if<std::shared_ptr<const node_initializer>>(&iter->second))
        continue;

      if (_access_init(iter)->name() == initializer.name()) {
        Info(
          g_logger, "Removed node_initializer: {}", _access_init(iter)->name());
        m_map.erase(iter++);
      } else
        ++iter;
    }
  }

  bool node_info_manager::exists(const std::string& name) const
  {
    auto lck = _lock();
    return m_map.find(name) != m_map.end();
  }

  auto node_info_manager::enumerate_info()
    -> std::vector<std::shared_ptr<const info_type>>
  {
    auto lck = _lock();

    std::vector<std::shared_ptr<const info_type>> ret;

    for (auto&& [name, var] : m_map) {
      if (auto ptr = std::get_if<std::shared_ptr<const node_info>>(&var))
        ret.push_back(*ptr);
    }

    return ret;
  }

  auto node_info_manager::enumerate_initializer()
    -> std::vector<std::shared_ptr<const node_initializer>>
  {
    auto lck = _lock();

    std::vector<std::shared_ptr<const node_initializer>> ret;

    for (auto&& [name, var] : m_map) {
      if (auto ptr = std::get_if<std::shared_ptr<const node_initializer>>(&var))
        ret.push_back(*ptr);
    }

    return ret;
  }

  auto node_info_manager::find_info(const std::string& name) const
    -> std::shared_ptr<const node_info>
  {
    auto lck = _lock();

    auto iter = m_map.find(name);

    if (iter == m_map.end())
      return nullptr;

    if (auto ptr = std::get_if<std::shared_ptr<const node_info>>(&iter->second))
      return *ptr;

    return nullptr;
  }

  auto node_info_manager::find_initializer(const std::string& name) const
    -> std::shared_ptr<const node_initializer>
  {
    auto lck = _lock();

    auto iter = m_map.find(name);

    if (iter == m_map.end())
      return nullptr;

    if (
      auto ptr =
        std::get_if<std::shared_ptr<const node_initializer>>(&iter->second))
      return *ptr;

    return nullptr;
  }

  void node_info_manager::clear()
  {
    auto lck = _lock();
    m_map.clear();
  }

  bool node_info_manager::empty() const
  {
    auto lck = _lock();
    return m_map.empty();
  }

  auto node_info_manager::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock(m_mtx);
  }

  auto node_info_manager::_access_init(const map_type::const_iterator& it) const
    -> std::shared_ptr<const node_initializer>
  {
    return *std::get_if<std::shared_ptr<const node_initializer>>(&it->second);
  }

  auto node_info_manager::_access_info(const map_type::const_iterator& it) const
    -> std::shared_ptr<const node_info>
  {
    return *std::get_if<std::shared_ptr<const node_info>>(&it->second);
  }

} // namespace yave