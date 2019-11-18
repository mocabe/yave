//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/managed_node_graph.hpp>
#include <yave/node/class/node_group.hpp>

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

      if (lb != contents.end() && *lb == node)
        return;

      contents.insert(lb, node);
    }

    void remove_content(const node_handle& node)
    {
      auto lb = std::lower_bound(contents.begin(), contents.end(), node);

      if (lb == contents.end())
        return;

      if (*lb == node)
        contents.erase(lb);
    }

    bool find_content(const node_handle& node) const
    {
      auto lb = std::lower_bound(contents.begin(), contents.end(), node);

      if (lb == contents.end())
        return false;

      return *lb == node;
    }
  };

  void managed_node_graph::_init()
  {
    assert(m_ng.empty());
    assert(m_nim.empty());
    assert(m_groups.empty());
    assert(!m_root_group);

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

  managed_node_graph::managed_node_graph()
    : m_ng {}
    , m_nim {}
    , m_groups {}
    , m_root_group {}
  {
    init_logger();
    _init();
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

  bool managed_node_graph::register_node_initializer(
    const node_initializer& initializer)
  {
    return m_nim.add(initializer);
  }

  void managed_node_graph::unregister_node_initializer(
    const node_initializer& initializer)
  {
    return m_nim.remove(initializer);
  }

  bool managed_node_graph::register_node_initializer(
    const std::vector<node_initializer>& initializers)
  {
    std::vector<const node_initializer*> added;

    bool succ = [&] {
      for (auto&& i : initializers) {
        if (!m_nim.add(i))
          return false;
        added.push_back(&i);
      }
      return true;
    }();

    if (succ)
      return true;

    for (auto&& pi : added) {
      m_nim.remove(*pi);
    }

    return false;
  }

  void managed_node_graph::unregister_node_initializer(
    const std::vector<node_initializer>& initializers)
  {
    for (auto&& i : initializers) {
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
        if (!parent->find_content(n)) {
          Error(
            g_logger,
            "group(): Invalid node in group member. id={}",
            to_string(n.id()));
          return {nullptr};
        }
      }
    }

    // create new group
    {
      // erase nodes from current group
      for (auto&& n : nodes) {
        parent->remove_content(n);
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
      parent->add_content(interface);

      // rebuild connections
      {
        // pair of socket and its outbound connections
        struct _out_cs
        {
          // socket
          socket_handle s;
          // outbound connections (can be empty)
          std::vector<std::pair<connection_handle, connection_info>> cs;
        };

        std::vector<_out_cs> inputs;
        std::vector<_out_cs> outputs;

        // find connections
        for (auto&& n : nodes) {

          auto is = m_ng.input_sockets(n);
          auto os = m_ng.output_sockets(n);

          // input
          for (auto&& s : is) {

            // add table
            inputs.push_back(_out_cs {s, {}});

            // collect outbound connections
            auto ic = m_ng.connections(s);
            for (auto&& c : ic) {
              auto info = m_ng.get_info(c);
              assert(info);
              if (
                std::find(nodes.begin(), nodes.end(), info->src_node()) ==
                nodes.end()) {
                inputs.back().cs.emplace_back(c, std::move(*info));
              }
            }
          }

          // output
          for (auto&& s : os) {

            // add table
            outputs.push_back(_out_cs {s, {}});

            // collect outbound connections
            auto oc = m_ng.connections(s);
            for (auto&& c : oc) {
              auto info = m_ng.get_info(c);
              assert(info);
              if (
                std::find(nodes.begin(), nodes.end(), info->dst_node()) ==
                nodes.end()) {
                outputs.back().cs.emplace_back(c, std::move(*info));
              }
            }
          }
        }

        // attach input handler
        for (auto&& [is, cs] : inputs) {

          // no outbound connection
          if (cs.empty())
            continue;

          // create I/O bit node
          auto bit_name = *m_ng.get_name(is);
          auto bit_info = get_node_info<node::NodeGroupIOBit>();
          bit_info.set_input_sockets({bit_name});
          bit_info.set_output_sockets({bit_name});
          auto bit = m_ng.add(bit_info);
          assert(bit);

          it->second.input_bits.push_back(bit);

          auto bit_in  = m_ng.input_sockets(bit)[0];
          auto bit_out = m_ng.output_sockets(bit)[0];

          // build connections
          for (auto&& [ic, icinfo] : cs) {

            m_ng.disconnect(ic);

            [[maybe_unused]] connection_handle c;
            [[maybe_unused]] bool b;

            // src -> bit
            c = m_ng.connect(icinfo.src_socket(), bit_in);
            assert(c);
            // bit -> dst
            c = m_ng.connect(bit_out, icinfo.dst_socket());
            assert(c);

            // attach input handler
            b = m_ng.attach_interface(it->second.input_handler, bit_out);
            assert(b);
            // attach interface
            b = m_ng.attach_interface(interface, bit_in);
            assert(b);
          }
        }

        for (auto&& [os, cs] : outputs) {

          // no outbound connection
          if (cs.empty())
            continue;

          // create I/O bit node
          auto bit_name = *m_ng.get_name(os);
          auto bit_info = get_node_info<node::NodeGroupIOBit>();
          bit_info.set_input_sockets({bit_name});
          bit_info.set_output_sockets({bit_name});
          auto bit = m_ng.add(bit_info);
          assert(bit);

          it->second.output_bits.push_back(bit);

          auto bit_in  = m_ng.input_sockets(bit)[0];
          auto bit_out = m_ng.output_sockets(bit)[0];

          for (auto&& [oc, ocinfo] : cs) {

            // build loop connection
            m_ng.disconnect(oc);

            [[maybe_unused]] connection_handle c;
            [[maybe_unused]] bool b;

            // src -> bit
            c = m_ng.connect(ocinfo.src_socket(), bit_in);
            assert(c);
            // bit -> dst
            c = m_ng.connect(bit_out, ocinfo.dst_socket());
            assert(c);

            // attach handler
            b = m_ng.attach_interface(it->second.output_handler, bit_in);
            assert(b);
            // attach interface
            b = m_ng.attach_interface(interface, bit_out);
            assert(b);
          }
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
      for (auto&& socket : m_ng.input_sockets(group->interface)) {

        auto bit = m_ng.get_owner(socket);
        auto ics = m_ng.input_connections(bit);
        auto ocs = m_ng.output_connections(bit);

        // no connection: discard
        if (ics.empty() || ocs.empty())
          continue;

        assert(ics.size() == 1);
        auto ici = m_ng.get_info(ics[0]);
        m_ng.disconnect(ics[0]);

        [[maybe_unused]] connection_handle c;

        for (auto&& oc : ocs) {
          auto oci = m_ng.get_info(oc);
          m_ng.disconnect(oc);
          c = m_ng.connect(ici->src_socket(), oci->dst_socket());
          assert(c);
        }
      }

      // output handler
      for (auto&& socket : m_ng.output_sockets(group->interface)) {

        auto bit = m_ng.get_owner(socket);
        auto ics = m_ng.input_connections(bit);
        auto ocs = m_ng.output_connections(bit);

        if (ics.empty() || ocs.empty())
          continue;

        assert(ics.size() == 1);
        auto ici = m_ng.get_info(ics[0]);
        m_ng.disconnect(ics[0]);

        [[maybe_unused]] connection_handle c;

        for (auto&& oc : ocs) {
          auto oci = m_ng.get_info(oc);
          m_ng.disconnect(oc);
          c = m_ng.connect(ici->src_socket(), oci->dst_socket());
          assert(c);
        }
      }
    }

    // remove interface from parent
    {
      parent->remove_content(node);
    }

    // move contents to parent
    {
      for (auto&& n : group->contents) {
        parent->add_content(n);
      }
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

  void managed_node_graph::set_group_name(
    const node_handle& group,
    const std::string& name)
  {
    if (!is_group(group))
      return;

    auto iter = m_groups.find(group);
    assert(iter != m_groups.end());

    m_ng.set_name(iter->second.interface, name);
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

    [[maybe_unused]] bool b;

    // attach
    for (auto&& bit : *pBits) {
      for (auto&& s : m_ng.input_sockets(bit)) {
        b = m_ng.attach_interface(*pInterfaceIn, s);
        assert(b);
      }
      for (auto&& s : m_ng.output_sockets(bit)) {
        b = m_ng.attach_interface(*pInterfaceOut, s);
        assert(b);
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
    std::vector<node_handle>* pBits;

    switch (type) {
      case socket_type::input:
        pBits = &group->input_bits;
        break;
      case socket_type::output:
        pBits = &group->output_bits;
        break;
      default:
        assert(false);
    }

    for (auto&& s : m_ng.input_sockets(pBits->operator[](index)))
      m_ng.set_name(s, socket);

    for (auto&& s : m_ng.output_sockets(pBits->operator[](index)))
      m_ng.set_name(s, socket);
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

    [[maybe_unused]] bool b;

    // attach
    {
      for (auto&& bit : *pBits) {
        for (auto&& s : m_ng.input_sockets(bit)) {
          b = m_ng.attach_interface(*pInterfaceIn, s);
          assert(b);
        }
        for (auto&& s : m_ng.output_sockets(bit)) {
          b = m_ng.attach_interface(*pInterfaceOut, s);
          assert(b);
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

  bool managed_node_graph::exists(const socket_handle& socket) const
  {
    return m_ng.exists(socket);
  }

  bool managed_node_graph::exists(const connection_handle& connection) const
  {
    return m_ng.exists(connection);
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

  auto managed_node_graph::get_name(const node_handle& node) const
    -> std::optional<std::string>
  {
    return m_ng.get_name(node);
  }

  auto managed_node_graph::get_name(const socket_handle& socket) const
    -> std::optional<std::string>
  {
    return m_ng.get_name(socket);
  }

  /* create/connect */

  auto managed_node_graph::create(
    const node_handle& parent_group,
    const std::string& name) -> node_handle
  {
    // info
    if (auto info = m_nim.find_info(name)) {

      // find group
      auto iter = m_groups.find(parent_group);
      if (iter == m_groups.end())
        return {nullptr};

      // create and add to node group
      auto node = m_ng.add(*info);
      assert(node);
      iter->second.add_content(node);

      Info(
        g_logger,
        "Create new node {} under group {}",
        to_string(node.id()),
        to_string(iter->second.interface.id()));

      return node;
    }

    // initializer
    if (auto init = m_nim.find_initializer(name)) {
      Info(g_logger, "Create node using initializer {}", init->name());
      return init->initialize(*this, parent_group);
    }

    return {nullptr};
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
    if (is_group(node)) {

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

      parent->remove_content(node);
      return;
    }

    // member
    m_ng.remove(node);
    parent->remove_content(node);
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

  /* sockets */

  auto managed_node_graph::input_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return m_ng.input_sockets(node);
  }

  auto managed_node_graph::output_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return m_ng.output_sockets(node);
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
    m_groups.clear();
    m_root_group = nullptr;

    _init();
  }

  auto managed_node_graph::get_node_graph() const -> const node_graph&
  {
    return m_ng;
  }

} // namespace yave