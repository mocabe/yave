//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/interface/root_manager.hpp>

namespace yave {

  root_manager::root_manager(node_graph& graph, bind_info_manager& binds)
    : m_graph {graph}
    , m_binds {binds}
  {
  }

  std::unique_lock<std::mutex> root_manager::lock() const
  {
    return std::unique_lock {m_mtx};
  }

  void root_manager::remove_root(const node_handle& node)
  {
    auto it = std::find_if(m_roots.begin(), m_roots.end(), [&](auto& table) {
      return table.node == node;
    });

    if (it != m_roots.end())
      m_roots.erase(it);
  }

  void root_manager::remove_root(const std::string& name)
  {
    auto it = std::find_if(m_roots.begin(), m_roots.end(), [&](auto& table) {
      return table.name == name;
    });

    if (it != m_roots.end())
      m_roots.erase(it);
  }

  bool root_manager::is_root(const node_handle& node) const
  {
    for (auto&& table : m_roots)
      if (node == table.node)
        return true;
    return false;
  }

  node_handle root_manager::find_root(const std::string& name) const
  {
    for (auto&& table : m_roots) {
      if (table.name == name)
        return table.node;
    }
    return nullptr;
  }

  object_ptr<const Type> root_manager::get_type(const node_handle& node) const
  {
    for (auto&& table : m_roots) {
      if (table.node == node)
        return table.type;
    }
    return nullptr;
  }

  object_ptr<const Type> root_manager::get_type(const std::string& name) const
  {
    for (auto&& table : m_roots) {
      if (table.name == name)
        return table.type;
    }
    return nullptr;
  }

} // namespace yave