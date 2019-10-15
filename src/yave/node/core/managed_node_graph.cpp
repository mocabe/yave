//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/managed_node_graph.hpp>
#include <yave/node/obj/node_group.hpp>

#include <yave/support/log.hpp>

#include <memory>

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

  /// Node group data.
  /// Node group is a way to create custom node which contains multiple nodes
  /// inside. Node gorup is implemented by 'interface' node feature of
  /// `node_graph`, which is a way to create a 'view' of sockets of other nodes.
  struct managed_node_graph::node_group
  {
    // back pointer to parent group
    node_group* parent;

    /// sorted list of nodes in this node group.
    std::vector<node_handle> contents;

    /// node represents this group (interface).
    /// `node::NodeGroupInterface`
    node_handle interface;

    /// input handler node (interface).
    /// `node::NodeGroupInput`
    node_handle input_handler;

    /// internal per-socket node objects.
    /// `node::NodeGroupIOBit`s
    std::vector<node_handle> input_bits;

    /// output handler node (interface).
    /// `node::NodeGroupOutout`
    node_handle output_handler;

    /// internal per-socket node objects.
    /// `node::NodeGroupIOBit`s
    std::vector<node_handle> output_bits;

    void add_content(const node_handle& node)
    {
      auto lb = std::lower_bound(contents.begin(), contents.end(), node);

      if (*lb == node)
        return;

      contents.insert(lb, node);
    }

    bool find_content(const node_handle& node) const
    {
      auto lb = std::lower_bound(contents.begin(), contents.end(), node);

      if (lb == contents.end())
        return false;

      return *lb == node;
    }
  };

  managed_node_graph::managed_node_graph()
    : m_ng {}
    , m_nim {}
    , m_groups {}
  {
    init_logger();

    // register group node info
    if (
      !m_nim.add(get_node_info<node::NodeGroupInterface>()) ||
      !m_nim.add(get_node_info<node::NodeGroupInput>()) ||
      !m_nim.add(get_node_info<node::NodeGroupOutput>()) ||
      !m_nim.add(get_node_info<node::NodeGroupIOBit>()))
      throw std::runtime_error("Failed to register node info");

    // initialize root group
    {
      m_root_group = m_ng.add(get_node_info<node::NodeGroupInterface>());

      assert(m_root_group);

      // root group doesn't have input/output handler
      [[maybe_unused]] auto [it, succ] = m_groups.emplace(
        m_root_group,
        node_group {nullptr, {}, m_root_group, {nullptr}, {}, {nullptr}, {}});

      assert(succ);
    }
  }

  managed_node_graph::~managed_node_graph() noexcept
  {
  }

  managed_node_graph::managed_node_graph(managed_node_graph&& other) noexcept
    : m_ng {}
    , m_nim {}
    , m_groups {}
    , m_root_group {}
  {
    m_ng         = std::move(other.m_ng);
    m_nim        = std::move(other.m_nim);
    m_groups     = std::move(other.m_groups);
    m_root_group = std::move(other.m_root_group);
  }

  managed_node_graph& managed_node_graph::operator=(
    managed_node_graph&& other) noexcept
  {
    m_ng         = std::move(other.m_ng);
    m_nim        = std::move(other.m_nim);
    m_groups     = std::move(other.m_groups);
    m_root_group = std::move(other.m_root_group);
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

  auto managed_node_graph::_find_parent_group(const node_handle& node) const
    -> const node_group*
  {
    if (!node)
      return nullptr;

    // find parent group of first node
    std::vector<node_handle> stack = {m_root_group};

    // DFS group tree
    while (!stack.empty()) {
      // pop
      auto gi = stack.back();
      stack.pop_back();

      auto iter = m_groups.find(gi);
      assert(iter != m_groups.end());

      auto* g = &iter->second;

      if (iter->second.input_handler == node)
        return g;

      if (iter->second.output_handler == node)
        return g;

      for (auto&& n : iter->second.input_bits) {
        (void)n;
        assert(n != node);
      }
      for (auto&& n : iter->second.output_bits) {
        (void)n;
        assert(n != node);
      }

      for (auto&& n : iter->second.contents) {

        if (n == node) {
          // found group
          return g;
        }

        // search child groups
        if (m_ng.is_interface(n)) {
          stack.push_back(n);
        }
      }
    }
    return nullptr;
  }

  auto managed_node_graph::_find_parent_group(const node_handle& node)
    -> node_group*
  {
    const auto* _this = this;
    auto cg           = _this->_find_parent_group(node);
    return const_cast<node_group*>(cg); // I'm sorry!
  }

  auto managed_node_graph::group(
    const node_handle& parent_group,
    const std::vector<node_handle>& nodes) -> node_handle
  {
    // get parent group
    node_group* parent = nullptr;
    {
      auto iter = m_groups.find(parent_group);
      if (iter == m_groups.end()) {
        Error(g_logger, "group(): Invalid parent handler");
        return {nullptr};
      }
      parent = &iter->second;
    }

    assert(parent);

    // check all other nodes are in the same group content
    {
      for (auto&& n : nodes) {

        auto lb =
          std::lower_bound(parent->contents.begin(), parent->contents.end(), n);

        // fail
        if (lb == parent->contents.end() || *lb != n) {
          Error(g_logger, "group(): Invalid node in group member");
          return {nullptr};
        }
      }
    }

    // create new group
    {
      // erase nodes from current group
      for (auto&& n : nodes) {
        auto lb =
          std::lower_bound(parent->contents.begin(), parent->contents.end(), n);
        parent->contents.erase(lb);
      }

      // create new group interfaces
      auto interface      = m_ng.add(get_node_info<node::NodeGroupInterface>());
      auto input_handler  = m_ng.add(get_node_info<node::NodeGroupInput>());
      auto output_handler = m_ng.add(get_node_info<node::NodeGroupOutput>());

      assert(interface);
      assert(input_handler);
      assert(output_handler);

      // I/O bits will be added later
      auto [it, succ] = m_groups.emplace(
        interface,
        node_group {
          parent, nodes, interface, input_handler, {}, output_handler, {}});

      assert(succ);

      // Add interface node to parent group
      parent->contents.push_back(interface);

      std::sort(it->second.contents.begin(), it->second.contents.end());

      // rebuild connections
      {
        // list of outbound connections
        std::vector<std::pair<connection_handle, connection_info>> inputs;
        std::vector<std::pair<connection_handle, connection_info>> outputs;

        // find connections
        for (auto&& n : nodes) {
          // input
          auto ic = m_ng.input_connections(n);
          for (auto&& c : ic) {
            auto info = m_ng.get_info(c);
            assert(info);

            if (
              std::find(nodes.begin(), nodes.end(), info->src_node()) ==
              nodes.end()) {
              inputs.emplace_back(c, std::move(*info));
            }
          }
          // output
          auto oc = m_ng.output_connections(n);
          for (auto&& c : oc) {
            auto info = m_ng.get_info(c);
            assert(info);

            if (
              std::find(nodes.begin(), nodes.end(), info->dst_node()) ==
              nodes.end()) {
              outputs.emplace_back(c, std::move(*info));
            }
          }
        }

        // attach input handler
        for (auto&& [c, cinfo] : inputs) {

          auto dst_socket = cinfo.dst_socket();
          auto dst_name   = *m_ng.get_name(cinfo.dst_socket());

          // create I/O bit node
          auto info = get_node_info<node::NodeGroupIOBit>();
          info.set_input_sockets({dst_name});
          info.set_output_sockets({dst_name});
          auto bit = m_ng.add(info);
          assert(bit);
          it->second.input_bits.push_back(bit);

          // build loop connection
          m_ng.disconnect(c);
          // src -> bit
          m_ng.connect(cinfo.src_socket(), m_ng.input_sockets(bit)[0]);
          // bit -> dst
          m_ng.connect(m_ng.output_sockets(bit)[0], cinfo.dst_socket());

          // attach input handler
          m_ng.attach_interface(
            it->second.input_handler, m_ng.output_sockets(bit)[0]);

          // attach interface
          m_ng.attach_interface(interface, m_ng.input_sockets(bit)[0]);
        }

        // attach output handler
        for (auto&& [c, cinfo] : outputs) {

          auto src_socket = cinfo.src_socket();
          auto src_name   = *m_ng.get_name(src_socket);

          // create I/O bit node
          auto info = get_node_info<node::NodeGroupIOBit>();
          info.set_input_sockets({src_name});
          info.set_output_sockets({src_name});
          auto bit = m_ng.add(info);
          assert(bit);
          it->second.output_bits.push_back(bit);

          // build loop connection
          m_ng.disconnect(c);
          // src -> bit
          m_ng.connect(cinfo.src_socket(), m_ng.input_sockets(bit)[0]);
          // bit -> dst
          m_ng.connect(m_ng.output_sockets(bit)[0], cinfo.dst_socket());

          // attach handler
          m_ng.attach_interface(
            it->second.output_handler, m_ng.input_sockets(bit)[0]);

          // attach interface
          m_ng.attach_interface(interface, m_ng.output_sockets(bit)[0]);
        }
      }

      Info(
        g_logger,
        "Created new node group: parent={}, id={}",
        to_string(parent->interface.id()),
        to_string(interface.id()));

      if (!nodes.empty())
        Info(g_logger, "Group members:");
      for (auto&& n : nodes) {
        Info(g_logger, "  {}", to_string(n.id()));
      }

      return interface;
    }
  }

  void managed_node_graph::ungroup(const node_handle& node)
  {
    if (!is_group(node))
      return;

    if (node == m_root_group)
      return;

    node_group* group  = nullptr;
    node_group* parent = nullptr;
    {
      auto iter = m_groups.find(node);
      assert(iter != m_groups.end());
      group  = &iter->second;
      parent = group->parent;
    }

    // rebuild connections
    {
      // input handler
      for (auto&& socket : m_ng.input_sockets(group->input_handler)) {

        auto bit = m_ng.get_owner(socket);
        auto ics = m_ng.input_connections(bit);
        auto ocs = m_ng.output_connections(bit);

        // no input: discard
        if (ics.empty())
          continue;

        assert(ics.size() == 1);
        auto ici = m_ng.get_info(ics.front());
        m_ng.disconnect(ics.front());

        for (auto&& oc : ocs) {
          auto oci = m_ng.get_info(oc);
          m_ng.disconnect(oc);
          m_ng.connect(ici->src_socket(), oci->dst_socket());
        }
      }

      // output handler
      for (auto&& socket : m_ng.input_sockets(group->output_handler)) {

        auto bit = m_ng.get_owner(socket);
        auto ics = m_ng.input_connections(bit);
        auto ocs = m_ng.output_connections(bit);

        if (ics.empty())
          continue;

        assert(ics.size() == 1);
        auto ici = m_ng.get_info(ics.front());
        m_ng.disconnect(ics.front());

        for (auto&& oc : ocs) {
          auto oci = m_ng.get_info(oc);
          m_ng.disconnect(oc);
          m_ng.connect(ici->src_socket(), oci->dst_socket());
        }
      }
    }

    // remove interface from parent
    {
      auto iter =
        std::find(parent->contents.begin(), parent->contents.end(), node);
      parent->contents.erase(iter);
    }

    // move contents to parent
    {
      for (auto&& n : group->contents) {
        parent->contents.push_back(n);
      }
      std::sort(parent->contents.begin(), parent->contents.end());
    }

    // remove interface, I/O bits, handlers
    {
      m_ng.remove(node);
      m_ng.remove(group->input_handler);
      m_ng.remove(group->output_handler);
      for (auto&& bit : group->input_bits) {
        m_ng.remove(bit);
      }
      for (auto&& bit : group->output_bits) {
        m_ng.remove(bit);
      }
    }

    // remove group
    {
      auto iter = m_groups.find(node);
      assert(iter != m_groups.end());
      m_groups.erase(iter);
    }
  }

  bool managed_node_graph::is_group(const node_handle& node) const
  {
    if (node == m_root_group)
      return true;

    if (!m_ng.is_interface(node))
      return false;

    return m_groups.find(node) != m_groups.end();
  }

  bool managed_node_graph::is_group_member(const node_handle& node) const
  {
    auto info = m_ng.get_info(node);

    if (info->name() == get_node_info<node::NodeGroupInput>().name())
      return false;

    if (info->name() == get_node_info<node::NodeGroupOutput>().name())
      return false;

    if (info->name() == get_node_info<node::NodeGroupInterface>().name())
      return false;

    assert(info->name() != get_node_info<node::NodeGroupIOBit>().name());

    return true;
  }

  bool managed_node_graph::is_group_output(const node_handle& node) const
  {
    if (auto info = m_ng.get_info(node)) {
      return info->name() == get_node_info<node::NodeGroupOutput>().name();
    }
    return false;
  }

  bool managed_node_graph::is_group_input(const node_handle& node) const
  {
    if (auto info = m_ng.get_info(node)) {
      return info->name() == get_node_info<node::NodeGroupInput>().name();
    }
    return false;
  }

  auto managed_node_graph::get_parent_group(const node_handle& node) const
    -> node_handle
  {
    auto g = _find_parent_group(node);

    if (!g)
      return {nullptr};

    return g->interface;
  }

  auto managed_node_graph::get_group_members(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!is_group(node))
      return {};

    auto iter = m_groups.find(node);
    assert(iter != m_groups.end());

    return iter->second.contents;
  }

  auto managed_node_graph::get_group_input(const node_handle& node) const
    -> node_handle
  {
    if (!is_group(node))
      return {nullptr};

    auto iter = m_groups.find(node);
    assert(iter != m_groups.end());

    return iter->second.input_handler;
  }

  auto managed_node_graph::get_group_output(const node_handle& node) const
    -> node_handle
  {
    if (!is_group(node))
      return {nullptr};

    auto iter = m_groups.find(node);
    assert(iter != m_groups.end());

    return iter->second.output_handler;
  }

  auto managed_node_graph::root_group() const -> node_handle
  {
    assert(m_ng.exists(m_root_group));
    return m_root_group;
  }

  bool managed_node_graph::_add_group_socket(
    node_group* group,
    socket_type type,
    const std::string& socket,
    size_t index)
  {
    node_handle* pInterfaceIn;
    node_handle* pInterfaceOut;
    std::vector<node_handle>* pBits;

    switch (type) {
      case socket_type::input:
        pInterfaceIn  = &group->interface;
        pInterfaceOut = &group->input_handler;
        pBits         = &group->input_bits;
        break;
      case socket_type::output:
        pInterfaceOut = &group->interface;
        pInterfaceIn  = &group->output_handler;
        pBits         = &group->output_bits;
        break;
      default:
        assert(false);
    }

    // detach
    for (auto&& bit : *pBits) {
      for (auto&& s : m_ng.input_sockets(bit)) {
        m_ng.detach_interface(*pInterfaceIn, s);
      }
      for (auto&& s : m_ng.output_sockets(bit)) {
        m_ng.detach_interface(*pInterfaceOut, s);
      }
    }

    // insert new bit
    {
      auto info = get_node_info<node::NodeGroupIOBit>();
      info.set_input_sockets({socket});
      info.set_output_sockets({socket});
      auto bit = m_ng.add(info);
      assert(bit);
      pBits->insert(pBits->begin() + index, bit);
    }

    // attach
    for (auto&& bit : *pBits) {
      for (auto&& s : m_ng.input_sockets(bit)) {
        m_ng.attach_interface(*pInterfaceIn, s);
      }
      for (auto&& s : m_ng.output_sockets(bit)) {
        m_ng.attach_interface(*pInterfaceOut, s);
      }
    }
    return true;
  }

  bool managed_node_graph::add_group_input_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index)
  {
    if (!is_group(group))
      return false;

    auto size = m_ng.input_sockets(group).size();

    if (index == (size_t)-1) {
      index = size;
    }

    if (size < index)
      return false;

    auto iter = m_groups.find(group);
    assert(iter != m_groups.end());

    return _add_group_socket(&iter->second, socket_type::input, socket, index);
  }

  bool managed_node_graph::add_group_output_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index)
  {
    if (!is_group(group))
      return false;

    auto size = m_ng.output_sockets(group).size();

    if (index == (size_t)-1) {
      index = size;
    }

    if (size < index)
      return false;

    auto iter = m_groups.find(group);
    assert(iter != m_groups.end());

    return _add_group_socket(&iter->second, socket_type::output, socket, index);
  }

  bool managed_node_graph::_set_group_socket(
    node_group* group,
    socket_type type,
    const std::string& socket,
    size_t index)
  {
    node_handle* pInterfaceIn;
    node_handle* pInterfaceOut;
    std::vector<node_handle>* pBits;

    switch (type) {
      case socket_type::input:
        pInterfaceIn  = &group->interface;
        pInterfaceOut = &group->input_handler;
        pBits         = &group->input_bits;
        break;
      case socket_type::output:
        pInterfaceOut = &group->interface;
        pInterfaceIn  = &group->output_handler;
        pBits         = &group->output_bits;
        break;
      default:
        assert(false);
    }

    // detach sockets
    {
      for (auto&& bit : *pBits) {
        for (auto&& s : m_ng.input_sockets(bit)) {
          m_ng.detach_interface(*pInterfaceIn, s);
        }
        for (auto&& s : m_ng.output_sockets(bit)) {
          m_ng.detach_interface(*pInterfaceOut, s);
        }
      }
    }

    // Create bit with new name and swap
    {
      node_handle bit;
      {
        auto info = get_node_info<node::NodeGroupIOBit>();
        info.set_input_sockets({socket});
        info.set_output_sockets({socket});
        bit = m_ng.add(info);
        assert(bit);
      }

      for (auto&& c : m_ng.input_connections(pBits->at(index))) {
        auto info = m_ng.get_info(c);
        m_ng.disconnect(c);
        m_ng.connect(info->src_socket(), m_ng.input_sockets(bit)[0]);
      }

      for (auto&& c : m_ng.output_connections(pBits->at(index))) {
        auto info = m_ng.get_info(c);
        m_ng.disconnect(c);
        m_ng.connect(m_ng.output_sockets(bit)[0], info->dst_socket());
      }

      // remove old bit
      std::swap(pBits->at(index), bit);
      m_ng.remove(bit);
    }

    // attach
    {
      for (auto&& bit : *pBits) {
        for (auto&& s : m_ng.input_sockets(bit)) {
          m_ng.attach_interface(*pInterfaceIn, s);
        }
        for (auto&& s : m_ng.output_sockets(bit)) {
          m_ng.attach_interface(*pInterfaceOut, s);
        }
      }
    }
    return true;
  }

  bool managed_node_graph::set_group_input_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index)
  {
    if (!is_group(group))
      return false;

    auto size = m_ng.input_sockets(group).size();

    if (index >= size)
      return false;

    auto iter = m_groups.find(group);
    assert(iter != m_groups.end());

    return _set_group_socket(&iter->second, socket_type::input, socket, index);
  }

  bool managed_node_graph::set_group_output_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index)
  {
    if (!is_group(group))
      return false;

    auto size = m_ng.output_sockets(group).size();

    if (index >= size)
      return false;

    auto iter = m_groups.find(group);
    assert(iter != m_groups.end());

    return _set_group_socket(&iter->second, socket_type::output, socket, index);
  }

  void managed_node_graph::_remove_group_socket(
    node_group* group,
    socket_type type,
    size_t index)
  {
    node_handle* pInterfaceIn;
    node_handle* pInterfaceOut;
    std::vector<node_handle>* pBits;

    switch (type) {
      case socket_type::input:
        pInterfaceIn  = &group->interface;
        pInterfaceOut = &group->input_handler;
        pBits         = &group->input_bits;
        break;
      case socket_type::output:
        pInterfaceOut = &group->interface;
        pInterfaceIn  = &group->output_handler;
        pBits         = &group->output_bits;
        break;
      default:
        assert(false);
    }

    // detach
    {
      for (auto&& bit : *pBits) {
        for (auto&& s : m_ng.input_sockets(bit)) {
          m_ng.detach_interface(*pInterfaceIn, s);
        }
        for (auto&& s : m_ng.output_sockets(bit)) {
          m_ng.detach_interface(*pInterfaceOut, s);
        }
      }
    }

    // remove
    {
      m_ng.remove(pBits->at(index));
      pBits->erase(pBits->begin() + index);
    }

    // attach
    {
      for (auto&& bit : *pBits) {
        for (auto&& s : m_ng.input_sockets(bit)) {
          m_ng.attach_interface(*pInterfaceIn, s);
        }
        for (auto&& s : m_ng.output_sockets(bit)) {
          m_ng.attach_interface(*pInterfaceOut, s);
        }
      }
    }
  }

  void managed_node_graph::remove_group_input_socket(
    const node_handle& group,
    size_t index)
  {
    if (!is_group(group))
      return;

    auto size = m_ng.input_sockets(group).size();

    if (index >= size)
      return;

    auto iter = m_groups.find(group);
    assert(iter != m_groups.end());

    return _remove_group_socket(&iter->second, socket_type::input, index);
  }

  void managed_node_graph::remove_group_output_socket(
    const node_handle& group,
    size_t index)
  {
    if (!is_group(group))
      return;

    auto size = m_ng.output_sockets(group).size();

    if (index >= size)
      return;

    auto iter = m_groups.find(group);
    assert(iter != m_groups.end());

    return _remove_group_socket(&iter->second, socket_type::output, index);
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

  auto managed_node_graph::create(
    const node_handle& parent_group,
    const std::string& name) -> node_handle
  {
    auto info = m_nim.find(name);

    if (!info)
      return nullptr;

    // find group
    auto iter = m_groups.find(parent_group);
    if (iter == m_groups.end())
      return {nullptr};

    // create and add to node group
    auto node = m_ng.add(*info);
    assert(node);
    iter->second.contents.insert(
      std::lower_bound(
        iter->second.contents.begin(), iter->second.contents.end(), node),
      node);

    Info(
      g_logger,
      "Create new node {} under group {}",
      to_string(node.id()),
      to_string(iter->second.interface.id()));

    return node;
  }

  auto managed_node_graph::create_shared(
    const node_handle& parent_group,
    const std::string& name) -> shared_node_handle
  {
    return shared_node_handle(*this, create(parent_group, name));
  }

  void managed_node_graph::destroy(const node_handle& node)
  {
    auto parent = _find_parent_group(node);

    if (!parent)
      return;

    // i/o handler
    if (parent->input_handler == node || parent->output_handler == node) {
      Warning(g_logger, "Tried to destroy group i/o handler, ignored.");
      return;
    }

    // group
    if (m_ng.is_interface(node)) {
      auto iter = m_groups.find(node);
      assert(iter != m_groups.end());

      // remove interface
      m_ng.remove(node);
      m_ng.remove(iter->second.input_handler);
      m_ng.remove(iter->second.output_handler);

      // remove contents
      for (auto&& n : iter->second.contents) {
        m_ng.remove(n);
      }

      parent->contents.erase(
        std::find(parent->contents.begin(), parent->contents.end(), node));
      return;
    }

    // member
    m_ng.remove(node);
    parent->contents.erase(
      std::find(parent->contents.begin(), parent->contents.end(), node));
    return;
  }

  auto managed_node_graph::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket) -> connection_handle
  {
    return m_ng.connect(src_socket, dst_socket);
  }

  void managed_node_graph::disconnect(const connection_handle& handle)
  {
    return m_ng.disconnect(handle);
  }

  /* stats */

  auto managed_node_graph::connections() const -> std::vector<connection_handle>
  {
    return m_ng.connections();
  }

  auto managed_node_graph::connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_ng.connections(node);
  }

  auto managed_node_graph::connections(const socket_handle& socket) const
    -> std::vector<connection_handle>
  {
    return m_ng.connections(socket);
  }

  auto managed_node_graph::input_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_ng.input_connections(node);
  }

  auto managed_node_graph::output_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_ng.output_connections(node);
  }

  auto managed_node_graph::get_info(const node_handle& node) const
    -> std::optional<node_info>
  {
    return m_ng.get_info(node);
  }

  auto managed_node_graph::get_info(const socket_handle& socket) const
    -> std::optional<socket_info>
  {
    return m_ng.get_info(socket);
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

    if (info->is_primitive()) {
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