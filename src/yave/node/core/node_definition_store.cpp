//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_definition_store.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(node_definition_store)

namespace yave {

  ////////////////////////////////////////
  // node_definition_map

  bool node_definition_map::add(const node_definition& def)
  {
    m_map.emplace(def.full_name(), std::make_shared<node_definition>(def));
    return true;
  }

  void node_definition_map::remove(const std::string& name)
  {
    auto [b, e] = m_map.equal_range(name);

    while (b != e) {
      b = m_map.erase(b);
    }
  }

  bool node_definition_map::exists(const std::string& name) const
  {
    auto iter = m_map.find(name);
    return iter != m_map.end();
  }

  bool node_definition_map::exists(const std::string& name, const size_t& os)
    const
  {
    auto [b, e] = m_map.equal_range(name);
    for (auto iter = b; iter != e; ++iter) {
      if (iter->second->output_socket() == os)
        return true;
    }
    return false;
  }

  auto node_definition_map::find(const std::string& name) const
    -> std::vector<std::shared_ptr<const node_definition>>
  {
    std::vector<std::shared_ptr<const node_definition>> ret;

    auto [b, e] = m_map.equal_range(name);

    for (auto iter = b; iter != e; ++iter) {
      ret.push_back(iter->second);
    }
    return ret;
  }

  auto node_definition_map::get_binds(const std::string& name, const size_t& os)
    const -> std::vector<std::shared_ptr<const node_definition>>
  {
    std::vector<std::shared_ptr<const node_definition>> ret;

    auto [b, e] = m_map.equal_range(name);

    for (auto iter = b; iter != e; ++iter) {
      if (iter->second->output_socket() == os)
        ret.push_back(iter->second);
    }
    return ret;
  }

  auto node_definition_map::size() const -> size_t
  {
    return m_map.size();
  }

  bool node_definition_map::empty() const
  {
    return m_map.empty();
  }

  void node_definition_map::clear()
  {
    m_map.clear();
  }

  ////////////////////////////////////////
  // node_definition_store

  node_definition_store::node_definition_store()
    : m_map {}
  {
  }

  bool node_definition_store::add(const node_definition& def)
  {
    return m_map.add(def);
  }

  bool node_definition_store::add(const std::vector<node_definition>& defs)
  {
    for (auto&& def : defs)
      add(def);

    return true;
  }

  void node_definition_store::remove(const std::string& name)
  {
    m_map.remove(name);
  }

  void node_definition_store::remove(const std::vector<std::string>& names)
  {
    for (auto&& name : names)
      remove(name);
  }

  bool node_definition_store::exists(const std::string& name) const
  {
    return m_map.exists(name);
  }

  bool node_definition_store::exists(const std::string& name, const size_t& os)
    const
  {
    return m_map.exists(name, os);
  }

  auto node_definition_store::find(const std::string& name) const
    -> std::vector<std::shared_ptr<const node_definition>>
  {
    return m_map.find(name);
  }

  auto node_definition_store::get_map() const -> const node_definition_map&
  {
    return m_map;
  }

  auto node_definition_store::size() const -> size_t
  {
    return m_map.size();
  }

  void node_definition_store::clear()
  {
    m_map.clear();
  }

  bool node_definition_store::empty() const
  {
    return m_map.empty();
  }

} // namespace yave