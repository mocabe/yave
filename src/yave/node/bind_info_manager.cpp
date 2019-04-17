//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/bind_info_manager.hpp>

namespace yave {

  BindInfoManager::BindInfoManager(const BindInfoManager& other)
    : m_info {other.m_info}
    , m_mtx {}
  {
  }

  BindInfoManager::BindInfoManager(BindInfoManager&& other)
    : m_info {std::move(other.m_info)}
    , m_mtx {}
  {
  }

  BindInfoManager& BindInfoManager::operator=(const BindInfoManager& other)
  {
    m_info = other.m_info;
    return *this;
  }

  BindInfoManager& BindInfoManager::operator=(BindInfoManager&& other)
  {
    m_info = std::move(other.m_info);
    return *this;
  }

  BindInfoManager::BindInfoManager()
    : m_info {}
    , m_mtx {}
  {
  }

  [[nodiscard]] bool BindInfoManager::add(const BindInfo& info)
  {
    auto iter = m_info.emplace(info.name(), info);
    return iter != m_info.end();
  }

  void BindInfoManager::remove_iterator(map_type::const_iterator iter)
  {
    m_info.erase(iter);
  }

  void BindInfoManager::remove(const std::string& name)
  {
    auto [bgn, end] = m_info.equal_range(name);
    auto iter       = bgn;
    while (iter != end) {
      remove_iterator(iter++);
    }
  }

  void BindInfoManager::remove(map_type::const_iterator iter)
  {
    iter = [&]() {
      for (auto it = m_info.begin(); it != m_info.end(); ++it) {
        if (iter == it)
          return iter;
      }
      return m_info.cend();
    }();
    if (iter == m_info.end())
      return;
    remove_iterator(iter);
  }

  [[nodiscard]] bool BindInfoManager::exists(const std::string& name) const
  {
    return m_info.find(name) != m_info.end();
  }

  void BindInfoManager::remove(
    const std::string& name,
    const std::vector<std::string> input_sockets,
    const std::string& output_socket)
  {

    auto iter = m_info.begin();
    auto end  = m_info.end();
    while (iter != end) {
      if (
        iter->second.name() == name &&
        iter->second.input_sockets() == input_sockets &&
        iter->second.output_socket() == output_socket) {
        remove_iterator(iter++);
      }
    }
  }

  [[nodiscard]] std::vector<const BindInfoManager::info_type*>
    BindInfoManager::find(const std::string& name) const
  {
    auto [bgn, end] = m_info.equal_range(name);
    std::vector<const info_type*> ret;
    for (auto iter = bgn; iter != end; ++iter) ret.push_back(&iter->second);
    return ret;
  }

  [[nodiscard]] BindInfoManager::const_iterator BindInfoManager::cbegin() const
  {
    return m_info.cbegin();
  }

  [[nodiscard]] BindInfoManager::const_iterator BindInfoManager::cend() const
  {
    return m_info.cend();
  }

  [[nodiscard]] std::unique_lock<std::mutex> BindInfoManager::lock() const
  {
    return std::unique_lock(m_mtx);
  }

  void BindInfoManager::clear()
  {
    m_info.clear();
  }

} // namespace yave