//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/node_info_manager.hpp>

#include <cassert>

namespace yave {

  node_info_manager::node_info_manager(const node_info_manager& other)
    : m_info {other.m_info}
    , m_mtx {}
  {
  }

  node_info_manager::node_info_manager(node_info_manager&& other)
    : m_info {std::move(other.m_info)}
    , m_mtx {}
  {
  }

  node_info_manager& node_info_manager::
    operator=(const node_info_manager& other)
  {
    m_info = other.m_info;
    return *this;
  }

  node_info_manager& node_info_manager::operator=(node_info_manager&& other)
  {
    m_info = std::move(other.m_info);
    return *this;
  }

  node_info_manager::node_info_manager()
    : m_info {}
    , m_mtx {}
  {
  }

  [[nodiscard]] bool node_info_manager::add(const info_type& info)
  {
    auto succ =
      m_info.emplace(info.name(), std::make_shared<info_type>(info)).second;
    return succ;
  }

  void node_info_manager::remove(const std::string& name)
  {
    // find
    auto it = m_info.find(name);
    // not found
    if (it == m_info.end())
      return;
    // remove
    m_info.erase(it);
  }

  void node_info_manager::remove(const node_info& info)
  {
    auto iter = m_info.begin();
    auto end  = m_info.end();
    while (iter != end) {
      if (*iter->second == info) {
        m_info.erase(iter++);
      }
    }
  }

  [[nodiscard]] bool node_info_manager::exists(const std::string& name) const
  {
    return m_info.find(name) != m_info.end();
  }

  [[nodiscard]] std::vector<std::shared_ptr<const node_info_manager::info_type>>
    node_info_manager::enumerate()
  {
    std::vector<std::shared_ptr<const info_type>> ret;
    for (auto&& [name, ptr] : m_info) {
      assert(name == ptr->name());
      ret.push_back(ptr);
    }
    return ret;
  }

  [[nodiscard]] std::shared_ptr<const node_info_manager::info_type>
    node_info_manager::find(const std::string& name) const
  {
    auto iter = m_info.find(name);
    if (iter == m_info.end())
      return nullptr;
    return iter->second;
  }

  [[nodiscard]] std::unique_lock<std::mutex> node_info_manager::lock() const
  {
    return std::unique_lock(m_mtx);
  }

  void node_info_manager::clear()
  {
    m_info.clear();
  }

} // namespace yave