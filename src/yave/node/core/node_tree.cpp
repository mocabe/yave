//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_tree.hpp>
#include <yave/support/log.hpp>

namespace {

  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("node_tree");
      return 1;
    }();
  }
} // namespace

namespace yave {

  node_tree::node_tree()
    : m_ng {}
    , m_nim {}
    , m_bim {}
  {
    init_logger();

    auto lck1 = m_ng.lock();
    auto lck3 = m_nim.lock();
    auto lck2 = m_bim.lock();

    auto prim_info = get_primitive_node_info_list();
    auto prim_bind = get_primitive_bind_info_list();

    for (auto&& info : prim_info) {
      [[maybe_unused]] auto r = m_nim.add(info);
      assert(r);
    }

    for (auto&& info : prim_bind) {
      [[maybe_unused]] auto r = m_bim.add(info);
      assert(r);
    }
  }

  node_tree::node_tree(const node_tree& other)
    : m_ng {}
    , m_nim {}
    , m_bim {}
  {
    auto lck1 = other.m_ng.lock();
    auto lck2 = other.m_nim.lock();
    auto lck3 = other.m_bim.lock();

    auto ng  = other.m_ng;
    auto nim = other.m_nim;
    auto bim = other.m_bim;

    m_ng  = std::move(ng);
    m_nim = std::move(nim);
    m_bim = std::move(bim);
  }

  node_tree::node_tree(node_tree&& other) noexcept
    : m_ng {}
    , m_nim {}
    , m_bim {}
  {
    auto lck1 = other.m_ng.lock();
    auto lck2 = other.m_nim.lock();
    auto lck3 = other.m_bim.lock();

    m_ng  = std::move(other.m_ng);
    m_nim = std::move(other.m_nim);
    m_bim = std::move(other.m_bim);
  }

  node_tree::~node_tree() noexcept
  {
  }

  node_tree& node_tree::operator=(const node_tree& other)
  {
    auto lck1 = other.m_ng.lock();
    auto lck2 = other.m_nim.lock();
    auto lck3 = other.m_bim.lock();

    m_ng  = other.m_ng;
    m_nim = other.m_nim;
    m_bim = other.m_bim;

    return *this;
  }

  node_tree& node_tree::operator=(node_tree&& other) noexcept
  {
    auto lck1 = other.m_ng.lock();
    auto lck2 = other.m_nim.lock();
    auto lck3 = other.m_bim.lock();

    m_ng  = std::move(other.m_ng);
    m_nim = std::move(other.m_nim);
    m_bim = std::move(other.m_bim);

    return *this;
  }

  /* reg/unreg */

  bool node_tree::register_node_info(const node_info& info)
  {
    auto lck = m_nim.lock();
    return m_nim.add(info);
  }

  bool node_tree::unregister_node_info(const node_info& info)
  {
    auto lck = m_nim.lock();
    m_nim.remove(info);
    return true;
  }

  bool node_tree::register_bind_info(const bind_info& info)
  {
    auto lck = m_bim.lock();
    return m_bim.add(info);
  }

  bool node_tree::unregister_bind_info(const bind_info& info)
  {
    auto lck = m_bim.lock();
    m_bim.remove(info);
    return true;
  }

  /* exists */

  bool node_tree::exists(const node_handle& node) const
  {
    auto lck = m_ng.lock();
    return m_ng.exists(node);
  }
  bool node_tree::exists(const connection_handle& connection) const
  {
    auto lck = m_ng.lock();
    return m_ng.exists(connection);
  }

  /* create/connect */

  node_handle node_tree::create(const std::string& name)
  {
    auto lck1 = m_nim.lock();
    auto lck2 = m_ng.lock();

    auto info = m_nim.find(name);

    if (!info)
      return nullptr;

    return m_ng.add(*info);
  }

  void node_tree::destroy(const node_handle& handle)
  {
    auto lck = m_ng.lock();
    return m_ng.remove(handle);
  }

  connection_handle node_tree::connect(
    const node_handle& src_n,
    const std::string& src_s,
    const node_handle& dst_n,
    const std::string& dst_s)
  {
    auto lck = m_ng.lock();
    return m_ng.connect(src_n, src_s, dst_n, dst_s);
  }

  void node_tree::disconnect(const connection_handle& handle)
  {
    auto lck = m_ng.lock();
    return m_ng.disconnect(handle);
  }

  /* stats */

  std::vector<node_handle> node_tree::nodes() const
  {
    auto lck = m_ng.lock();
    return m_ng.nodes();
  }

  std::vector<connection_handle> node_tree::connections() const
  {
    auto lck = m_ng.lock();
    return m_ng.connections();
  }

  std::vector<connection_handle>
    node_tree::connections(const node_handle& node) const
  {
    auto lck = m_ng.lock();
    return m_ng.connections(node);
  }

  std::vector<connection_handle> node_tree::connections(
    const node_handle& node,
    const std::string& socket) const
  {
    auto lck = m_ng.lock();
    return m_ng.connections(node, socket);
  }

  std::vector<connection_handle> node_tree::input_connections() const
  {
    auto lck = m_ng.lock();
    return m_ng.input_connections();
  }

  std::vector<connection_handle>
    node_tree::input_connections(const node_handle& node) const
  {
    auto lck = m_ng.lock();
    return m_ng.input_connections(node);
  }

  std::vector<connection_handle> node_tree::input_connections(
    const node_handle& node,
    const std::string& socket) const
  {
    auto lck = m_ng.lock();
    return m_ng.input_connections(node, socket);
  }

  std::vector<connection_handle> node_tree::output_connections() const
  {
    auto lck = m_ng.lock();
    return m_ng.output_connections();
  }

  std::vector<connection_handle>
    node_tree::output_connections(const node_handle& node) const
  {
    auto lck = m_ng.lock();
    return m_ng.output_connections(node);
  }

  std::vector<connection_handle> node_tree::output_connections(
    const node_handle& node,
    const std::string& socket) const
  {
    auto lck = m_ng.lock();
    return m_ng.output_connections(node, socket);
  }

  std::optional<node_info> node_tree::get_info(const node_handle& node) const
  {
    auto lck = m_ng.lock();
    return m_ng.get_info(node);
  }

  std::optional<connection_info>
    node_tree::get_info(const connection_handle& connection) const
  {
    auto lck = m_ng.lock();
    return m_ng.get_info(connection);
  }

  std::optional<primitive_t>
    node_tree::get_primitive(const node_handle& node) const
  {
    auto lck = m_ng.lock();
    return m_ng.get_primitive(node);
  }

  bool
    node_tree::set_primitive(const node_handle& node, const primitive_t& prim)
  {
    auto lck = m_ng.lock();

    auto info = m_ng.get_info(node);

    if (!info)
      return false;

    if (info->is_prim()) {
      m_ng.set_primitive(node, prim);
      return true;
    }

    return false;
  }

  void node_tree::clear()
  {
    auto lck1 = m_ng.lock();
    auto lck2 = m_nim.lock();
    auto lck3 = m_bim.lock();

    m_ng.clear();
    m_nim.clear();
    m_bim.clear();

    for (auto&& info : get_primitive_node_info_list()) {
      [[maybe_unused]] auto r = m_nim.add(info);
      assert(r);
    }

    for (auto&& info : get_primitive_bind_info_list()) {
      [[maybe_unused]] auto r = m_bim.add(info);
      assert(r);
    }
  }

} // namespace yave