//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/scene/managed_node_graph.hpp>
#include <yave/support/log.hpp>

namespace {

  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("managed_node_graph");
      return 1;
    }();
  }
} // namespace

namespace yave {

  managed_node_graph::managed_node_graph()
    : m_ng {}
    , m_nim {}
  {
    init_logger();
  }

  managed_node_graph::managed_node_graph(const managed_node_graph& other)
    : m_ng {}
    , m_nim {}
  {
    auto ng  = other.m_ng;
    auto nim = other.m_nim;

    m_ng  = std::move(ng);
    m_nim = std::move(nim);
  }

  managed_node_graph::managed_node_graph(managed_node_graph&& other) noexcept
    : m_ng {}
    , m_nim {}
  {
    m_ng  = std::move(other.m_ng);
    m_nim = std::move(other.m_nim);
  }

  managed_node_graph::~managed_node_graph() noexcept
  {
  }

  managed_node_graph& managed_node_graph::operator=(
    const managed_node_graph& other)
  {
    m_ng  = other.m_ng;
    m_nim = other.m_nim;

    return *this;
  }

  managed_node_graph& managed_node_graph::operator=(
    managed_node_graph&& other) noexcept
  {
    m_ng  = std::move(other.m_ng);
    m_nim = std::move(other.m_nim);

    return *this;
  }

  /* reg/unreg */

  bool managed_node_graph::register_node_info(const node_info& info)
  {
    return m_nim.add(info);
  }

  void managed_node_graph::unregister_node_info(const node_info& info)
  {
    return m_nim.remove(info);
  }

  bool managed_node_graph::register_node_info(
    const std::vector<node_info>& info)
  {
    std::vector<const node_info*> added;

    bool succ = [&] {
      for (auto&& i : info) {
        if (!m_nim.add(i))
          return false;
        added.push_back(&i);
      }
      return true;
    }();

    // added all info
    if (succ)
      return true;

    // rollback changes
    for (auto&& pi : added) {
      m_nim.remove(*pi);
    }

    return false;
  }

  void managed_node_graph::unregister_node_info(
    const std::vector<node_info>& info)
  {
    for (auto&& i : info) {
      m_nim.remove(i);
    }
  }

  /* exists */

  bool managed_node_graph::exists(const node_handle& node) const
  {
    return m_ng.exists(node);
  }
  bool managed_node_graph::exists(const connection_handle& connection) const
  {
    return m_ng.exists(connection);
  }

  /* create/connect */

  auto managed_node_graph::create(const std::string& name) -> node_handle
  {
    auto info = m_nim.find(name);

    if (!info)
      return nullptr;

    return m_ng.add(*info);
  }

  auto managed_node_graph::create_shared(const std::string& name)
    -> shared_node_handle
  {
    return shared_node_handle(m_ng, create(name));
  }

  void managed_node_graph::destroy(const node_handle& handle)
  {
    return m_ng.remove(handle);
  }

  auto managed_node_graph::connect(
    const node_handle& src_n,
    const std::string& src_s,
    const node_handle& dst_n,
    const std::string& dst_s) -> connection_handle
  {
    return m_ng.connect(src_n, src_s, dst_n, dst_s);
  }

  void managed_node_graph::disconnect(const connection_handle& handle)
  {
    return m_ng.disconnect(handle);
  }

  /* stats */

  auto managed_node_graph::nodes() const -> std::vector<node_handle>
  {
    return m_ng.nodes();
  }

  auto managed_node_graph::connections() const -> std::vector<connection_handle>
  {
    return m_ng.connections();
  }

  auto managed_node_graph::connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_ng.connections(node);
  }

  auto managed_node_graph::connections(
    const node_handle& node,
    const std::string& socket) const -> std::vector<connection_handle>
  {
    return m_ng.connections(node, socket);
  }

  auto managed_node_graph::input_connections() const
    -> std::vector<connection_handle>
  {
    return m_ng.input_connections();
  }

  auto managed_node_graph::input_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_ng.input_connections(node);
  }

  auto managed_node_graph::input_connections(
    const node_handle& node,
    const std::string& socket) const -> std::vector<connection_handle>
  {
    return m_ng.input_connections(node, socket);
  }

  auto managed_node_graph::output_connections() const
    -> std::vector<connection_handle>
  {
    return m_ng.output_connections();
  }

  auto managed_node_graph::output_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_ng.output_connections(node);
  }

  auto managed_node_graph::output_connections(
    const node_handle& node,
    const std::string& socket) const -> std::vector<connection_handle>
  {
    return m_ng.output_connections(node, socket);
  }

  auto managed_node_graph::get_info(const node_handle& node) const
    -> std::optional<node_info>
  {
    return m_ng.get_info(node);
  }

  auto managed_node_graph::get_info(const connection_handle& connection) const
    -> std::optional<connection_info>
  {
    return m_ng.get_info(connection);
  }

  auto managed_node_graph::get_primitive(const node_handle& node) const
    -> std::optional<primitive_t>
  {
    return m_ng.get_primitive(node);
  }

  bool managed_node_graph::set_primitive(
    const node_handle& node,
    const primitive_t& prim)
  {
    auto info = m_ng.get_info(node);

    if (!info)
      return false;

    if (info->is_prim()) {
      m_ng.set_primitive(node, prim);
      return true;
    }

    return false;
  }

  void managed_node_graph::clear()
  {
    m_ng.clear();
    m_nim.clear();
  }

  auto managed_node_graph::get_node_graph() const -> const node_graph&
  {
    return m_ng;
  }

} // namespace yave