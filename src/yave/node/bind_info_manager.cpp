//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/bind_info_manager.hpp>
#include <yave/tools/log.hpp>

namespace yave {

  BindInfoManager::BindInfoManager()
    : m_info {}
    , m_mtx {}
  {
  }

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

  [[nodiscard]] bool BindInfoManager::add(const BindInfo& info)
  {
    auto iter = m_info.emplace(info.name(), std::make_shared<info_type>(info));
    return iter != m_info.end();
  }

  void BindInfoManager::remove(const std::string& name)
  {
    auto [bgn, end] = m_info.equal_range(name);

    auto iter = bgn;
    while (iter != end) {
      assert(iter->second->name() == name);
      m_info.erase(iter++);
    }
  }

  void BindInfoManager::remove(
    const std::string& name,
    const std::vector<std::string>& input,
    const std::string& output)
  {
    auto [bgn, end] = m_info.equal_range(name);

    auto iter = bgn;
    while (iter != end) {
      assert(iter->second->name() == name);
      if (
        iter->second->output_socket() == output &&
        iter->second->input_sockets() == input)
        m_info.erase(iter++);
      else
        ++iter;
    }
  }

  [[nodiscard]] bool BindInfoManager::exists(const std::string& name) const
  {
    return m_info.find(name) != m_info.end();
  }

  [[nodiscard]] std::vector<std::shared_ptr<const BindInfoManager::info_type>>
    BindInfoManager::enumerate()
  {
    std::vector<std::shared_ptr<const info_type>> ret;
    for (auto&& [name, ptr] : m_info) {
      assert(name == ptr->name());
      ret.push_back(ptr);
    }
    return ret;
  }

  [[nodiscard]] std::vector<std::shared_ptr<const BindInfoManager::info_type>>
    BindInfoManager::find(const std::string& name) const
  {
    auto [bgn, end] = m_info.equal_range(name);

    std::vector<std::shared_ptr<const info_type>> ret;
    for (auto iter = bgn; iter != end; ++iter) {
      ret.push_back(iter->second);
    }
    return ret;
  }

  [[nodiscard]] std::vector<std::shared_ptr<const BindInfoManager::info_type>>
    BindInfoManager::find(
      const std::string& name,
      const std::vector<std::string>& input,
      const std::string& output) const
  {
    auto [bgn, end] = m_info.equal_range(name);

    std::vector<std::shared_ptr<const info_type>> ret;
    for (auto iter = bgn; iter != end; ++iter) {
      if (
        iter->second->output_socket() == output &&
        iter->second->input_sockets() == input)
        ret.push_back(iter->second);
    }
    return ret;
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