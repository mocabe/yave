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
    auto lck2 = other.m_nim.lock();

    auto ng  = other.m_ng;
    auto nim = other.m_nim;

    m_ng  = std::move(ng);
    m_nim = std::move(nim);
  }

  managed_node_graph::managed_node_graph(managed_node_graph&& other) noexcept
    : m_ng {}
    , m_nim {}
  {
    auto lck2 = other.m_nim.lock();

    m_ng  = std::move(other.m_ng);
    m_nim = std::move(other.m_nim);
  }

  managed_node_graph::~managed_node_graph() noexcept
  {
  }

  managed_node_graph& managed_node_graph::
    operator=(const managed_node_graph& other)
  {
    auto lck2 = other.m_nim.lock();

    m_ng  = other.m_ng;
    m_nim = other.m_nim;

    return *this;
  }

  managed_node_graph& managed_node_graph::
    operator=(managed_node_graph&& other) noexcept
  {
    auto lck2 = other.m_nim.lock();

    m_ng  = std::move(other.m_ng);
    m_nim = std::move(other.m_nim);

    return *this;
  }

  /* reg/unreg */

  bool managed_node_graph::register_node_info(const node_info& info)
  {
    auto lck = m_nim.lock();
    return m_nim.add(info);
  }

  void managed_node_graph::unregister_node_info(const node_info& info)
  {
    auto lck = m_nim.lock();
    return m_nim.remove(info);
  }

  bool
    managed_node_graph::register_node_info(const std::vector<node_info>& info)
  {
    auto lck = m_nim.lock();

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

  void
    managed_node_graph::unregister_node_info(const std::vector<node_info>& info)
  {
    auto lck = m_nim.lock();

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

  node_handle managed_node_graph::create(const std::string& name)
  {
    auto lck1 = m_nim.lock();

    auto info = m_nim.find(name);

    if (!info)
      return nullptr;

    return m_ng.add(*info);
  }

  shared_node_handle managed_node_graph::create_shared(const std::string& name)
  {
    return shared_node_handle(m_ng, create(name));
  }

  void managed_node_graph::destroy(const node_handle& handle)
  {
    return m_ng.remove(handle);
  }

  connection_handle managed_node_graph::connect(
    const node_handle& src_n,
    const std::string& src_s,
    const node_handle& dst_n,
    const std::string& dst_s)
  {
    return m_ng.connect(src_n, src_s, dst_n, dst_s);
  }

  void managed_node_graph::disconnect(const connection_handle& handle)
  {
    return m_ng.disconnect(handle);
  }

  /* stats */

  std::vector<node_handle> managed_node_graph::nodes() const
  {
    return m_ng.nodes();
  }

  std::vector<connection_handle> managed_node_graph::connections() const
  {
    return m_ng.connections();
  }

  std::vector<connection_handle>
    managed_node_graph::connections(const node_handle& node) const
  {
    return m_ng.connections(node);
  }

  std::vector<connection_handle> managed_node_graph::connections(
    const node_handle& node,
    const std::string& socket) const
  {
    return m_ng.connections(node, socket);
  }

  std::vector<connection_handle> managed_node_graph::input_connections() const
  {
    return m_ng.input_connections();
  }

  std::vector<connection_handle>
    managed_node_graph::input_connections(const node_handle& node) const
  {
    return m_ng.input_connections(node);
  }

  std::vector<connection_handle> managed_node_graph::input_connections(
    const node_handle& node,
    const std::string& socket) const
  {
    return m_ng.input_connections(node, socket);
  }

  std::vector<connection_handle> managed_node_graph::output_connections() const
  {
    return m_ng.output_connections();
  }

  std::vector<connection_handle>
    managed_node_graph::output_connections(const node_handle& node) const
  {
    return m_ng.output_connections(node);
  }

  std::vector<connection_handle> managed_node_graph::output_connections(
    const node_handle& node,
    const std::string& socket) const
  {
    return m_ng.output_connections(node, socket);
  }

  std::optional<node_info>
    managed_node_graph::get_info(const node_handle& node) const
  {
    return m_ng.get_info(node);
  }

  std::optional<connection_info>
    managed_node_graph::get_info(const connection_handle& connection) const
  {
    return m_ng.get_info(connection);
  }

  std::optional<primitive_t>
    managed_node_graph::get_primitive(const node_handle& node) const
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
    auto lck2 = m_nim.lock();

    m_ng.clear();
    m_nim.clear();
  }

  const node_graph& managed_node_graph::get_node_graph() const
  {
    return m_ng;
  }

} // namespace yave