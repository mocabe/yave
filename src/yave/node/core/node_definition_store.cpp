//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_definition_store.hpp>
#include <yave/support/log.hpp>

namespace {

  // logger
  std::shared_ptr<spdlog::logger> g_logger;

  // init
  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("node_definition_store");
      return 1;
    }();
  }
} // namespace

namespace yave {

  node_definition_store::node_definition_store()
    : m_map {}
  {
  }

  node_definition_store::node_definition_store(
    const node_definition_store& other)
    : m_map {other.m_map}
  {
  }

  node_definition_store::node_definition_store(
    node_definition_store&& other) noexcept
    : m_map {std::move(other.m_map)}
  {
  }

  node_definition_store& node_definition_store::operator=(
    const node_definition_store& other)
  {
    m_map = other.m_map;
  }

  node_definition_store& node_definition_store::operator=(
    node_definition_store&& other) noexcept
  {
    m_map = std::move(other.m_map);
  }

  void node_definition_store::add(const node_definition& def)
  {
    m_map.emplace(def.name(), std::make_shared<node_definition>(def));
  }

  void node_definition_store::add(const std::vector<node_definition>& defs)
  {
    for (auto&& def : defs) {
      add(def);
    }
  }

  bool node_definition_store::exists(const std::string& name) const
  {
    auto iter = m_map.find(name);
    return iter != m_map.end();
  }

  bool node_definition_store::exists(
    const std::string& name,
    const std::string& os) const
  {
    auto [b, e] = m_map.equal_range(name);
    for (auto iter = b; iter != e; ++iter) {
      if (iter->second->output_socket() == os)
        return true;
    }
    return false;
  }

  auto node_definition_store::find(const std::string& name) const
    -> std::vector<std::shared_ptr<node_definition>>
  {
    std::vector<std::shared_ptr<node_definition>> ret;

    auto [b, e] = m_map.equal_range(name);

    for (auto iter = b; iter != e; ++iter) {
      ret.push_back(iter->second);
    }
    return ret;
  }

  auto node_definition_store::get_binds(
    const std::string& name,
    const std::string& os) const
    -> std::vector<std::shared_ptr<node_definition>>
  {
    std::vector<std::shared_ptr<node_definition>> ret;

    auto [b, e] = m_map.equal_range(name);

    for (auto iter = b; iter != e; ++iter) {
      if (iter->second->output_socket() == os)
        ret.push_back(iter->second);
    }
    return ret;
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