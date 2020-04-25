//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/managed_node_graph.hpp>
#include <yave/node/core/node_group.hpp>
#include <yave/rts/dynamic_typing.hpp>

#include <yave/support/log.hpp>

#include <range/v3/algorithm.hpp>
#include <memory>

YAVE_DECL_G_LOGGER(managed_node_graph)

namespace yave {

  using namespace ranges;

  namespace {

    /// internal data structure for node groups
    struct node_group
    {
      // back pointer to parent group
      node_group* parent;

      /// sorted list of nodes in this node group.
      std::vector<node_handle> contents;

      /// node represents this group (interface).
      /// `node::NodeGroupInterface`
      node_handle interface;

      /// input handler node visible in this group (interface).
      /// `node::NodeGroupInput`
      node_handle input_handler;

      /// output handler node visible in this group (interface).
      /// `node::NodeGroupOutout`
      node_handle output_handler;

      /// internal per-socket node objects.
      /// `node::NodeGroupIOBit`s
      std::vector<node_handle> input_bits;

      /// internal per-socket node objects.
      /// `node::NodeGroupIOBit`s
      std::vector<node_handle> output_bits;

    public:
      void add_content(const node_handle& n)
      {
        assert(!has_content(n));
        contents.push_back(n);
      }

      void remove_content(const node_handle& n)
      {
        contents.erase(find(contents, n));
        assert(find(contents, n) == contents.end());
      }

      bool has_content(const node_handle& n)
      {
        return find(contents, n) != contents.end();
      }

      void bring_front(const node_handle& n)
      {
        assert(has_content(n));
        auto end = ranges::remove(contents, n);
        contents.erase(end, contents.end());
        contents.insert(contents.end(), n);
      }

      void bring_back(const node_handle& n)
      {
        assert(has_content(n));
        auto end = ranges::remove(contents, n);
        contents.erase(end, contents.end());
        contents.insert(contents.begin(), n);
      }
    };

    struct node_data
    {
      /// position
      tvec2<float> pos;
    };

    struct socket_data
    {
      /// custom data
      object_ptr<Object> data;
    };

    /// internal metadata object
    using NodeData = Box<node_data>;
    /// internal metadata object
    using SocketData = Box<socket_data>;

  } // namespace

  class managed_node_graph::impl
  {
  public:
    /// node graph
    yave::node_graph ng;

  public:
    /// map of groups
    std::map<node_handle, node_group> groups;
    /// root group
    node_handle root_group;

  public:
    void init()
    {
      assert(ng.empty());
      assert(groups.empty());
      assert(!root_group);

      // initialize root group
      {
        auto g_decl = get_node_declaration<node::NodeGroupInterface>();
        auto i_decl = get_node_declaration<node::NodeGroupInput>();
        auto o_decl = get_node_declaration<node::NodeGroupOutput>();

        auto root_g = ng.add(g_decl.name(), {}, {}, node_type::interface);
        auto root_i = ng.add(i_decl.name(), {}, {}, node_type::interface);
        auto root_o = ng.add(o_decl.name(), {}, {}, node_type::interface);

        if (!root_g || !root_i || !root_o)
          throw std::runtime_error("Failed to create root group interface");

        // group
        groups.emplace(
          root_g, node_group {nullptr, {}, root_g, root_i, root_o, {}, {}});

        ng.set_data(root_g, make_object<NodeData>());
        ng.set_data(root_i, make_object<NodeData>());
        ng.set_data(root_o, make_object<NodeData>());

        root_group = root_g;
      }
    }

    impl()
    {
      init_logger();
      init();
    }

    impl(std::nullptr_t)
    {
    }

    impl(impl&& other) = default;

  public:
    bool exists(const node_handle& node) const
    {
      return ng.exists(node);
    }
    bool exists(const socket_handle& socket) const
    {
      return ng.exists(socket);
    }
    bool exists(const connection_handle& connection) const
    {
      return ng.exists(connection);
    }

    auto node(const uid& id) const
    {
      return ng.node(id);
    }
    auto socket(const uid& id) const
    {
      return ng.socket(id);
    }
    auto connection(const uid& id) const
    {
      return ng.connection(id);
    }

  public:
    auto get_name(const node_handle& node) const
    {
      return ng.get_name(node);
    }

    auto get_name(const socket_handle& socket) const
    {
      return ng.get_name(socket);
    }

    auto get_pos(const node_handle& node) const
    {
      auto data = ng.get_data(node);

      assert(has_type<NodeData>(data));

      return value_cast_if<NodeData>(data)->pos;
    }

    void set_pos(const node_handle& node, const tvec2<float>& new_pos)
    {
      auto data = ng.get_data(node);

      assert(has_type<NodeData>(data));

      value_cast_if<NodeData>(data)->pos = new_pos;
    }

  public:
    auto get_info(const node_handle& node) const
    {
      auto info = *ng.get_info(node);

      auto type = [&] {
        switch (info.type()) {
          case node_type::normal:
            return managed_node_type::normal;
          case node_type::interface:
            if (
              info.name()
              == get_node_declaration<node::NodeGroupInput>().name())
              return managed_node_type::group_input;
            if (
              info.name()
              == get_node_declaration<node::NodeGroupOutput>().name())
              return managed_node_type::group_output;
            if (
              info.name()
              == get_node_declaration<node::NodeGroupInterface>().name())
              return managed_node_type::group;
            unreachable();
          default:
            unreachable();
        }
      }();

      auto data = ng.get_data(node);

      assert(value_cast_if<NodeData>(data));

      return managed_node_info(
        info.name(),
        info.input_sockets(),
        info.output_sockets(),
        type,
        value_cast_if<NodeData>(data)->pos);
    }

    auto get_info(const socket_handle& socket) const
    {
      auto info = *ng.get_info(socket);

      auto n = info.interfaces().empty() ? info.node() : info.interfaces()[0];
      return managed_socket_info(info.name(), info.type(), n);
    }

    auto get_info(const connection_handle& connection) const
    {
      auto info = *ng.get_info(connection);

      auto srcn = info.src_node();
      auto dstn = info.dst_node();

      if (!info.src_interfaces().empty()) {
        assert(info.src_interfaces().size() == 1);
        srcn = info.src_interfaces()[0];
      }

      if (!info.dst_interfaces().empty()) {
        assert(info.dst_interfaces().size() == 1);
        dstn = info.dst_interfaces()[0];
      }

      return managed_connection_info(
        srcn, info.src_socket(), dstn, info.dst_socket());
    }

    auto get_data(const socket_handle& h) const
    {
      auto sd = ng.get_data(h);

      if (!sd)
        return object_ptr();

      assert(has_type<SocketData>(sd));
      return value_cast_if<SocketData>(sd)->data;
    }

    void set_data(const socket_handle& h, const object_ptr<Object>& new_data)
    {
      auto sd = ng.get_data(h);

      if (!sd) {
        ng.set_data(h, make_object<SocketData>(new_data));
        return;
      }

      assert(has_type<SocketData>(sd));

      auto& data = value_cast_if<SocketData>(sd)->data;

      if (!same_type(get_type(data), get_type(new_data))) {
        Warning(
          "Assigning different type of data to socket: id={}, old={}, "
          "new={}",
          to_string(h.id()),
          to_string(get_type(data)),
          to_string(get_type(new_data)));
      }
      data = new_data;
    }

  public:
    auto node(const socket_handle& socket) const
    {
      auto is = ng.interfaces(socket);
      if (!is.empty()) {
        assert(is.size() == 1);
        return is[0];
      }

      return ng.node(socket);
    }

    auto nodes() const
    {
      std::vector<node_handle> ret;

      for (auto&& n : ng.nodes()) {

        // ignore I/O bits
        if (
          ng.get_info(n)->name()
          == get_node_declaration<node::NodeGroupIOBit>().name())
          continue;

        ret.push_back(n);
      }

      return ret;
    }

    auto nodes(const node_handle& group) const
    {
      auto iter = groups.find(group);

      std::vector<node_handle> ret;

      if (iter == groups.end())
        return ret;

      // I/O
      ret.push_back(iter->second.input_handler);
      ret.push_back(iter->second.output_handler);

      // member
      ret.insert(
        ret.end(), iter->second.contents.begin(), iter->second.contents.end());

      return ret;
    }

    auto input_sockets(const node_handle& node) const
    {
      return ng.input_sockets(node);
    }

    auto output_sockets(const node_handle& node) const
    {
      return ng.output_sockets(node);
    }

    auto connections() const
    {
      return ng.connections();
    }

    auto connections(const node_handle& node) const
    {
      return ng.connections(node);
    }

    auto connections(const socket_handle& socket) const
    {
      return ng.connections(socket);
    }

    auto input_connections(const node_handle& node) const
    {
      return ng.input_connections(node);
    }

    auto output_connections(const node_handle& node) const
    {
      return ng.output_connections(node);
    }

  public:
    auto find_parent_group(const node_handle& node) const -> const node_group*
    {
      for (auto& [n, g] : groups) {
        if (g.interface == node) {
          return g.parent;
        }
        if (g.input_handler == node)
          return &g;
        if (g.output_handler == node)
          return &g;
        for (auto&& c : g.contents) {
          if (c == node)
            return &g;
        }
      }
      unreachable();
    }

    auto* find_parent_group(const node_handle& node)
    {
      const auto* cthis = this;
      return const_cast<node_group*>(cthis->find_parent_group(node));
    }

    bool is_group(const node_handle& node) const
    {
      if (node == root_group)
        return true;

      if (!ng.is_interface(node))
        return false;

      return groups.find(node) != groups.end();
    }

    bool is_group_member(const node_handle& node) const
    {
      if (auto name = ng.get_name(node)) {

        if (*name == get_node_declaration<node::NodeGroupInput>().name())
          return false;

        if (*name == get_node_declaration<node::NodeGroupOutput>().name())
          return false;

        if (*name == get_node_declaration<node::NodeGroupInterface>().name())
          return false;

        assert(*name != get_node_declaration<node::NodeGroupIOBit>().name());

        return true;
      }
      return false;
    }

    bool is_group_output(const node_handle& node) const
    {
      if (auto name = ng.get_name(node))
        return *name == get_node_declaration<node::NodeGroupOutput>().name();

      return false;
    }

    bool is_group_input(const node_handle& node) const
    {
      if (auto name = ng.get_name(node))
        return *name == get_node_declaration<node::NodeGroupInput>().name();

      return false;
    }

    auto get_parent_group(const node_handle& node) const
    {
      auto g = find_parent_group(node);

      if (!g)
        return node_handle();

      return g->interface;
    }

    auto get_group_members(const node_handle& node) const
      -> std::vector<node_handle>
    {
      if (!is_group(node))
        return {};

      auto iter = groups.find(node);
      assert(iter != groups.end());

      return iter->second.contents;
    }

    auto get_group_input(const node_handle& node) const
    {
      if (!is_group(node))
        return node_handle();

      auto iter = groups.find(node);
      assert(iter != groups.end());

      return iter->second.input_handler;
    }

    auto get_group_output(const node_handle& node) const
    {
      if (!is_group(node))
        return node_handle();

      auto iter = groups.find(node);
      assert(iter != groups.end());

      return iter->second.output_handler;
    }

  public:
    auto create(const node_handle& parent_group, const node_declaration& decl)
      -> node_handle
    {
      // find group
      node_group* group;
      {
        auto iter = groups.find(parent_group);

        if (iter == groups.end()) {
          Error(
            g_logger,
            "Failed to create node: Invalid group handler: {}",
            to_string(parent_group.id()));
          return {nullptr};
        }
        group = &iter->second;
      }

      // create node
      auto node = ng.add(
        decl.name(),
        decl.input_sockets(),
        decl.output_sockets(),
        node_type::normal);

      // all valid node_declarations are valid to create new node.
      assert(node);

      // create and add to node group
      group->add_content(node);

      ng.set_data(node, make_object<NodeData>());

      Info(
        g_logger,
        "Create new node {} under group {}",
        to_string(node.id()),
        to_string(group->interface.id()));

      // set default arguments
      auto is = ng.input_sockets(node);
      for (auto&& [idx, defval] : decl.default_arg()) {
        set_data(is[idx], defval.clone());
      }

      return node;
    }

    void destroy(const node_handle& node)
    {
      auto parent = find_parent_group(node);

      if (!parent)
        return;

      // i/o handler
      if (parent->input_handler == node || parent->output_handler == node) {
        Warning(g_logger, "Tried to destroy group i/o handler, ignored.");
        return;
      }

      // group
      if (is_group(node)) {

        auto iter = groups.find(node);
        assert(iter != groups.end());

        // remove interface
        ng.remove(node);
        ng.remove(iter->second.input_handler);
        ng.remove(iter->second.output_handler);

        // remove contents
        for (auto&& n : iter->second.contents) {
          ng.remove(n);
        }

        parent->remove_content(node);
        return;
      }

      // member
      ng.remove(node);
      parent->remove_content(node);
      return;
    }

  public:
    auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket) -> connection_handle
    {
      auto c  = ng.connect(src_socket, dst_socket);
      auto cs = ng.connections(dst_socket);

      if (cs.size() > 1) {
        assert(cs[1] == c);
        ng.disconnect(c);
        Error(g_logger, "Failed to connect: Multiple inputs are not allowed");
        return {};
      }
      return c;
    }

    void disconnect(const connection_handle& handle)
    {
      return ng.disconnect(handle);
    }

  public:
    auto group(
      const node_handle& parent_group,
      const std::vector<node_handle>& nodes)
    {
      // get parent group
      node_group* parent;
      {
        auto iter = groups.find(parent_group);

        if (iter == groups.end()) {
          Error(g_logger, "group(): Invalid parent handler");
          return node_handle();
        }
        parent = &iter->second;
      }

      // check all other nodes are in the same group content
      {
        for (auto&& n : nodes) {
          if (!parent->has_content(n)) {
            Error(
              g_logger,
              "group(): Invalid node in group member. id={}",
              to_string(n.id()));
            return node_handle();
          }
        }
      }

      // create new group
      {
        auto g_decl = get_node_declaration<node::NodeGroupInterface>();
        auto i_decl = get_node_declaration<node::NodeGroupInput>();
        auto o_decl = get_node_declaration<node::NodeGroupOutput>();

        // create new group interfaces
        auto node_g = ng.add(g_decl.name(), {}, {}, node_type::interface);
        auto node_i = ng.add(i_decl.name(), {}, {}, node_type::interface);
        auto node_o = ng.add(o_decl.name(), {}, {}, node_type::interface);

        if (!node_g || !node_i || !node_o)
          throw std::runtime_error("Failed to create new node group");

        // group (IO bits will be added later)
        auto [it, succ] = groups.emplace(
          node_g, node_group {parent, nodes, node_g, node_i, node_o, {}, {}});

        assert(succ);

        ng.set_data(node_g, make_object<NodeData>());
        ng.set_data(node_i, make_object<NodeData>());
        ng.set_data(node_o, make_object<NodeData>());

        // Add interface node to parent group
        parent->add_content(node_g);

        // erase nodes from current group
        for (auto&& n : nodes) {
          parent->remove_content(n);
        }

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

            auto is = ng.input_sockets(n);
            auto os = ng.output_sockets(n);

            // input
            for (auto&& s : is) {

              // add table
              inputs.push_back(_out_cs {s, {}});

              // collect outbound connections
              auto ic = ng.connections(s);
              for (auto&& c : ic) {
                auto info = ng.get_info(c);
                assert(info);
                if (find(nodes, info->src_node()) == nodes.end()) {
                  inputs.back().cs.emplace_back(c, std::move(*info));
                }
              }
            }

            // output
            for (auto&& s : os) {

              // add table
              outputs.push_back(_out_cs {s, {}});

              // collect outbound connections
              auto oc = ng.connections(s);
              for (auto&& c : oc) {
                auto info = ng.get_info(c);
                assert(info);
                if (find(nodes, info->dst_node()) == nodes.end()) {
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
            auto bit_decl   = get_node_declaration<node::NodeGroupIOBit>();
            auto bit_socket = *ng.get_name(is);

            auto bit = ng.add(
              bit_decl.name(), {bit_socket}, {bit_socket}, node_type::normal);

            if (!bit)
              throw std::runtime_error("Failed to create group IO bit");

            it->second.input_bits.push_back(bit);

            auto bit_in  = ng.input_sockets(bit)[0];
            auto bit_out = ng.output_sockets(bit)[0];

            // attach input handler
            auto b1 = ng.attach_interface(it->second.input_handler, bit_out);
            // attach interface
            auto b2 = ng.attach_interface(node_g, bit_in);

            if (!b1 || !b2)
              throw std::runtime_error("Failed to attach IO bit");

            // build connections
            for (auto&& [ic, icinfo] : cs) {

              ng.disconnect(ic);

              // src -> bit
              auto c1 = ng.connect(icinfo.src_socket(), bit_in);
              // bit -> dst
              auto c2 = ng.connect(bit_out, icinfo.dst_socket());

              if (!c1 || !c2)
                throw std::runtime_error("Failed to build group connection");
            }
          }

          for (auto&& [os, cs] : outputs) {

            // no outbound connection
            if (cs.empty())
              continue;

            // create I/O bit node
            auto bit_decl   = get_node_declaration<node::NodeGroupIOBit>();
            auto bit_socket = *ng.get_name(os);

            auto bit = ng.add(
              bit_decl.name(), {bit_socket}, {bit_socket}, node_type::normal);

            if (!bit)
              throw std::runtime_error("Failed to create group IO bit");

            it->second.output_bits.push_back(bit);

            auto bit_in  = ng.input_sockets(bit)[0];
            auto bit_out = ng.output_sockets(bit)[0];

            // attach handler
            auto b1 = ng.attach_interface(it->second.output_handler, bit_in);
            // attach interface
            auto b2 = ng.attach_interface(node_g, bit_out);

            if (!b1 || !b2)
              throw std::runtime_error("Failed to attach IO bit");

            // build connection
            for (auto&& [oc, ocinfo] : cs) {

              ng.disconnect(oc);

              // src -> bit
              auto c1 = ng.connect(ocinfo.src_socket(), bit_in);
              // bit -> dst
              auto c2 = ng.connect(bit_out, ocinfo.dst_socket());

              if (!c1 || !c2)
                throw std::runtime_error("Failed to build group connection");
            }
          }
        }

        Info(
          g_logger,
          "Created new node group: parent={}, id={}",
          to_string(parent->interface.id()),
          to_string(node_g.id()));

        if (!nodes.empty())
          Info(g_logger, "Group members:");
        for (auto&& n : nodes) {
          Info(g_logger, "  {}", to_string(n.id()));
        }

        return node_g;
      }
    }

    void ungroup(const node_handle& node)
    {
      if (!is_group(node))
        return;

      if (node == root_group)
        return;

      node_group* group  = nullptr;
      node_group* parent = nullptr;
      {
        auto iter = groups.find(node);
        assert(iter != groups.end());
        group  = &iter->second;
        parent = group->parent;
      }

      // rebuild connections
      {
        // input handler
        for (auto&& socket : ng.input_sockets(group->interface)) {

          auto bit = ng.get_owner(socket);
          auto ics = ng.input_connections(bit);
          auto ocs = ng.output_connections(bit);

          // no connection: discard
          if (ics.empty() || ocs.empty())
            continue;

          assert(ics.size() == 1);
          auto ici = ng.get_info(ics[0]);
          ng.disconnect(ics[0]);

          for (auto&& oc : ocs) {
            auto oci = ng.get_info(oc);

            ng.disconnect(oc);

            auto c = ng.connect(ici->src_socket(), oci->dst_socket());

            if (!c)
              throw std::runtime_error("Failed to ungroup");
          }
        }

        // output handler
        for (auto&& socket : ng.output_sockets(group->interface)) {

          auto bit = ng.get_owner(socket);
          auto ics = ng.input_connections(bit);
          auto ocs = ng.output_connections(bit);

          if (ics.empty() || ocs.empty())
            continue;

          assert(ics.size() == 1);
          auto ici = ng.get_info(ics[0]);
          ng.disconnect(ics[0]);

          for (auto&& oc : ocs) {
            auto oci = ng.get_info(oc);

            ng.disconnect(oc);

            auto c = ng.connect(ici->src_socket(), oci->dst_socket());

            if (!c)
              throw std::runtime_error("Failed to ungroup");
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
        ng.remove(node);
        ng.remove(group->input_handler);
        ng.remove(group->output_handler);
        for (auto&& bit : group->input_bits) {
          ng.remove(bit);
        }
        for (auto&& bit : group->output_bits) {
          ng.remove(bit);
        }
      }

      // remove group
      {
        auto iter = groups.find(node);
        assert(iter != groups.end());
        groups.erase(iter);
      }
    }

    auto add_group_socket(
      node_group* group,
      socket_type type,
      const std::string& socket,
      size_t index) -> socket_handle
    {
      node_handle* pInterfaceIn       = nullptr;
      node_handle* pInterfaceOut      = nullptr;
      std::vector<node_handle>* pBits = nullptr;

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
          unreachable();
      }

      // detach
      for (auto&& bit : *pBits) {
        for (auto&& s : ng.input_sockets(bit)) {
          ng.detach_interface(*pInterfaceIn, s);
        }
        for (auto&& s : ng.output_sockets(bit)) {
          ng.detach_interface(*pInterfaceOut, s);
        }
      }

      // insert new bit
      node_handle newbit;
      {
        auto bit_decl = get_node_declaration<node::NodeGroupIOBit>();
        newbit = ng.add(bit_decl.name(), {socket}, {socket}, node_type::normal);
        pBits->insert(pBits->begin() + index, newbit);
      }

      if (!newbit)
        throw std::runtime_error("Failed to add IO bit");

      // attach
      for (auto&& bit : *pBits) {
        {
          assert(ng.input_sockets(bit).size() == 1);
          auto attached =
            ng.attach_interface(*pInterfaceIn, ng.input_sockets(bit)[0]);
          if (!attached)
            throw std::runtime_error("Failed to attach IO bit");
        }
        {
          assert(ng.output_sockets(bit).size() == 1);
          auto attached =
            ng.attach_interface(*pInterfaceOut, ng.output_sockets(bit)[0]);
          if (!attached)
            throw std::runtime_error("Failed to attach IO bit");
        }
      }

      // return socket for interface
      switch (type) {
        case socket_type::input:
          return ng.input_sockets(newbit)[0];
        case socket_type::output:
          return ng.output_sockets(newbit)[0];
      }

      return {nullptr};
    }

    bool set_group_socket(
      node_group* group,
      socket_type type,
      const std::string& socket,
      size_t index)
    {
      std::vector<node_handle>* pBits = nullptr;

      switch (type) {
        case socket_type::input:
          pBits = &group->input_bits;
          break;
        case socket_type::output:
          pBits = &group->output_bits;
          break;
        default:
          unreachable();
      }

      for (auto&& s : ng.input_sockets(pBits->operator[](index)))
        ng.set_name(s, socket);

      for (auto&& s : ng.output_sockets(pBits->operator[](index)))
        ng.set_name(s, socket);
      return true;
    }

    void remove_group_socket(node_group* group, socket_type type, size_t index)
    {
      node_handle* pInterfaceIn       = nullptr;
      node_handle* pInterfaceOut      = nullptr;
      std::vector<node_handle>* pBits = nullptr;

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
          unreachable();
      }

      // detach
      {
        for (auto&& bit : *pBits) {
          for (auto&& s : ng.input_sockets(bit)) {
            ng.detach_interface(*pInterfaceIn, s);
          }
          for (auto&& s : ng.output_sockets(bit)) {
            ng.detach_interface(*pInterfaceOut, s);
          }
        }
      }

      // remove
      {
        ng.remove(pBits->at(index));
        pBits->erase(pBits->begin() + index);
      }

      // attach
      {
        for (auto&& bit : *pBits) {
          for (auto&& s : ng.input_sockets(bit)) {
            auto b = ng.attach_interface(*pInterfaceIn, s);

            if (!b)
              throw std::runtime_error("Failed to attach IO bit");
          }
          for (auto&& s : ng.output_sockets(bit)) {
            auto b = ng.attach_interface(*pInterfaceOut, s);

            if (!b)
              throw std::runtime_error("Failed to attach IO bit");
          }
        }
      }
    }

  public:
    auto add_group_input_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index) -> socket_handle
    {
      if (!is_group(group))
        return {nullptr};

      auto size = ng.input_sockets(group).size();

      if (index == (size_t)-1) {
        index = size;
      }

      if (size < index)
        return {nullptr};

      auto iter = groups.find(group);
      assert(iter != groups.end());

      return add_group_socket(&iter->second, socket_type::input, socket, index);
    }

    auto add_group_output_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index) -> socket_handle
    {
      if (!is_group(group))
        return {nullptr};

      auto size = ng.output_sockets(group).size();

      if (index == (size_t)-1) {
        index = size;
      }

      if (size < index)
        return {nullptr};

      auto iter = groups.find(group);
      assert(iter != groups.end());

      return add_group_socket(
        &iter->second, socket_type::output, socket, index);
    }

    void remove_group_input_socket(const node_handle& group, size_t index)
    {
      if (!is_group(group))
        return;

      auto size = ng.input_sockets(group).size();

      if (index >= size)
        return;

      auto iter = groups.find(group);
      assert(iter != groups.end());

      return remove_group_socket(&iter->second, socket_type::input, index);
    }

    void remove_group_output_socket(const node_handle& group, size_t index)
    {
      if (!is_group(group))
        return;

      auto size = ng.output_sockets(group).size();

      if (index >= size)
        return;

      auto iter = groups.find(group);
      assert(iter != groups.end());

      return remove_group_socket(&iter->second, socket_type::output, index);
    }

  public:
    void set_group_name(const node_handle& group, const std::string& name)
    {
      if (!is_group(group))
        return;

      auto iter = groups.find(group);
      assert(iter != groups.end());

      ng.set_name(iter->second.interface, name);
    }

    bool set_group_input_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index)
    {
      if (!is_group(group))
        return false;

      auto size = ng.input_sockets(group).size();

      if (index >= size)
        return false;

      auto iter = groups.find(group);
      assert(iter != groups.end());

      return set_group_socket(&iter->second, socket_type::input, socket, index);
    }

    bool set_group_output_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index)
    {
      if (!is_group(group))
        return false;

      auto size = ng.output_sockets(group).size();

      if (index >= size)
        return false;

      auto iter = groups.find(group);
      assert(iter != groups.end());

      return set_group_socket(
        &iter->second, socket_type::output, socket, index);
    }

  public:
    auto get_group_socket_inside(const socket_handle& socket) const
      -> socket_handle
    {
      auto info = get_info(socket);

      if (!is_group(info.node()))
        return {nullptr};

      auto bit = ng.get_info(socket)->node();

      assert(
        *ng.get_name(bit)
        == get_node_declaration<node::NodeGroupIOBit>().name());

      if (info.type() == socket_type::input) {
        return ng.output_sockets(bit)[0];
      }

      if (info.type() == socket_type::output) {
        return ng.input_sockets(bit)[0];
      }

      unreachable();
    }

    auto get_group_socket_outside(const socket_handle& socket) const
      -> socket_handle
    {
      auto info = get_info(socket);

      if (!is_group_input(info.node()) && !is_group_output(info.node()))
        return {nullptr};

      auto bit = ng.get_info(socket)->node();

      assert(
        *ng.get_name(bit)
        == get_node_declaration<node::NodeGroupIOBit>().name());

      if (info.type() == socket_type::input) {
        return ng.output_sockets(bit)[0];
      }

      if (info.type() == socket_type::output) {
        return ng.input_sockets(bit)[0];
      }

      unreachable();
    }

  public:
    void bring_front(const node_handle& node)
    {
      auto parent = find_parent_group(node);

      if (!parent)
        return;

      if (!parent->has_content(node))
        return;

      parent->bring_front(node);
    }

    void bring_back(const node_handle& node)
    {
      auto parent = find_parent_group(node);

      if (!parent)
        return;

      if (!parent->has_content(node))
        return;

      parent->bring_back(node);
    }

    void clear()
    {
      ng.clear();
      groups.clear();
      root_group = nullptr;
      init();
    }

    auto clone() const -> impl
    {
      Info(
        g_logger,
        "clone(): n={}, s={}, c={}",
        ng.nodes().size(),
        ng.sockets().size(),
        ng.connections().size());

      impl ret(nullptr);

      ret.ng = ng.clone();

      ret.root_group = ret.ng.node(root_group.id());

      ret.groups.clear();

      // mapping from old parent to new
      std::map<const node_group*, node_group*> group_map;

      // copy node groups
      for (auto&& [n, g] : groups) {

        auto newn = ret.ng.node(n.id());

        auto newg           = g;
        newg.interface      = ret.ng.node(newg.interface.id());
        newg.input_handler  = ret.ng.node(newg.input_handler.id());
        newg.output_handler = ret.ng.node(newg.output_handler.id());

        for (auto&& content : newg.contents) {
          content = ret.ng.node(content.id());
        }

        for (auto&& bit : newg.input_bits) {
          bit = ret.ng.node(bit.id());
        }

        for (auto&& bit : newg.output_bits) {
          bit = ret.ng.node(bit.id());
        }

        auto [it, succ] = ret.groups.emplace(std::move(newn), std::move(newg));

        assert(succ);

        if (!group_map.emplace(&g, &it->second).second)
          assert(false);
      }

      // fix parent pointers
      for (auto&& [n, g] : ret.groups) {

        // root
        if (!g.parent) {
          assert(g.interface == ret.root_group);
          continue;
        }

        auto iter = group_map.find(g.parent);

        assert(iter != group_map.end());

        g.parent = iter->second;
      }

      return ret;
    }
  };

  managed_node_graph::managed_node_graph()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  managed_node_graph::~managed_node_graph() noexcept
  {
  }

  managed_node_graph::managed_node_graph(managed_node_graph&& other) noexcept =
    default;

  managed_node_graph& managed_node_graph::operator=(
    managed_node_graph&& other) noexcept = default;

  /* reg/unreg */

  auto managed_node_graph::group(
    const node_handle& parent_group,
    const std::vector<node_handle>& nodes) -> node_handle
  {
    if (!exists(parent_group))
      return {};

    return m_pimpl->group(parent_group, nodes);
  }

  void managed_node_graph::ungroup(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->ungroup(node);
  }

  bool managed_node_graph::is_group(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group(node);
  }

  bool managed_node_graph::is_group_member(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_member(node);
  }

  bool managed_node_graph::is_group_output(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_output(node);
  }

  bool managed_node_graph::is_group_input(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_input(node);
  }

  auto managed_node_graph::get_parent_group(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return node_handle();

    return m_pimpl->get_parent_group(node);
  }

  auto managed_node_graph::get_group_members(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_members(node);
  }

  auto managed_node_graph::get_group_input(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_input(node);
  }

  auto managed_node_graph::get_group_output(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_output(node);
  }

  auto managed_node_graph::root_group() const -> node_handle
  {
    return m_pimpl->root_group;
  }

  auto managed_node_graph::add_group_input_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index) -> socket_handle
  {
    if (!exists(group))
      return {};

    return m_pimpl->add_group_input_socket(group, socket, index);
  }

  auto managed_node_graph::add_group_output_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index) -> socket_handle
  {
    if (!exists(group))
      return {};

    return m_pimpl->add_group_output_socket(group, socket, index);
  }

  void managed_node_graph::remove_group_input_socket(
    const node_handle& group,
    size_t index)
  {
    if (!exists(group))
      return;

    m_pimpl->remove_group_input_socket(group, index);
  }

  void managed_node_graph::remove_group_output_socket(
    const node_handle& group,
    size_t index)
  {
    if (!exists(group))
      return;

    m_pimpl->remove_group_output_socket(group, index);
  }

  void managed_node_graph::set_group_name(
    const node_handle& group,
    const std::string& name)
  {
    if (!exists(group))
      return;

    m_pimpl->set_group_name(group, name);
  }

  bool managed_node_graph::set_group_input_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index)
  {
    if (!exists(group))
      return false;

    return m_pimpl->set_group_input_socket(group, socket, index);
  }

  bool managed_node_graph::set_group_output_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index)
  {
    if (!exists(group))
      return false;

    return m_pimpl->set_group_output_socket(group, socket, index);
  }

  auto managed_node_graph::get_group_socket_inside(
    const socket_handle& socket) const -> socket_handle
  {
    if (!exists(socket))
      return {nullptr};

    return m_pimpl->get_group_socket_inside(socket);
  }

  auto managed_node_graph::get_group_socket_outside(
    const socket_handle& socket) const -> socket_handle
  {
    if (!exists(socket))
      return {nullptr};

    return m_pimpl->get_group_socket_outside(socket);
  }

  /* exists */

  bool managed_node_graph::exists(const node_handle& node) const
  {
    return m_pimpl->exists(node);
  }
  bool managed_node_graph::exists(const socket_handle& socket) const
  {
    return m_pimpl->exists(socket);
  }
  bool managed_node_graph::exists(const connection_handle& connection) const
  {
    return m_pimpl->exists(connection);
  }

  auto managed_node_graph::node(const uid& id) const -> node_handle
  {
    return m_pimpl->node(id);
  }
  auto managed_node_graph::socket(const uid& id) const -> socket_handle
  {
    return m_pimpl->socket(id);
  }
  auto managed_node_graph::connection(const uid& id) const -> connection_handle
  {
    return m_pimpl->connection(id);
  }

  auto managed_node_graph::get_info(const node_handle& node) const
    -> std::optional<managed_node_info>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_info(node);
  }

  auto managed_node_graph::get_info(const socket_handle& socket) const
    -> std::optional<managed_socket_info>
  {
    if (!exists(socket))
      return std::nullopt;

    return m_pimpl->get_info(socket);
  }

  auto managed_node_graph::get_info(const connection_handle& connection) const
    -> std::optional<managed_connection_info>
  {
    if (!exists(connection))
      return std::nullopt;

    return m_pimpl->get_info(connection);
  }

  auto managed_node_graph::get_name(const node_handle& node) const
    -> std::optional<std::string>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_name(node);
  }

  auto managed_node_graph::get_name(const socket_handle& socket) const
    -> std::optional<std::string>
  {
    if (!exists(socket))
      return std::nullopt;

    return m_pimpl->get_name(socket);
  }

  auto managed_node_graph::get_pos(const node_handle& node) const
    -> std::optional<tvec2<float>>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_pos(node);
  }

  void managed_node_graph::set_pos(
    const node_handle& node,
    const tvec2<float>& new_pos)
  {
    if (!exists(node))
      return;

    return m_pimpl->set_pos(node, new_pos);
  }

  auto managed_node_graph::create(
    const node_handle& parent_group,
    const node_declaration& decl) -> node_handle
  {
    if (!exists(parent_group))
      return {};

    return m_pimpl->create(parent_group, decl);
  }

  void managed_node_graph::destroy(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->destroy(node);
  }

  auto managed_node_graph::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket) -> connection_handle
  {
    if (!exists(src_socket) || !exists(dst_socket))
      return {};

    return m_pimpl->connect(src_socket, dst_socket);
  }

  void managed_node_graph::disconnect(const connection_handle& h)
  {
    if (!exists(h))
      return;

    return m_pimpl->disconnect(h);
  }

  /* member order */

  void managed_node_graph::bring_front(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->bring_front(node);
  }

  void managed_node_graph::bring_back(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->bring_back(node);
  }

  /* nodes */

  auto managed_node_graph::node(const socket_handle& socket) const
    -> node_handle
  {
    if (!exists(socket))
      return {nullptr};

    return m_pimpl->node(socket);
  }

  auto managed_node_graph::nodes() const -> std::vector<node_handle>
  {
    return m_pimpl->nodes();
  }

  auto managed_node_graph::nodes(const node_handle& group) const
    -> std::vector<node_handle>
  {
    if (!exists(group))
      return {};

    return m_pimpl->nodes(group);
  }

  /* sockets */

  auto managed_node_graph::input_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->input_sockets(node);
  }

  auto managed_node_graph::output_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->output_sockets(node);
  }

  /* stats */

  auto managed_node_graph::connections() const -> std::vector<connection_handle>
  {
    return m_pimpl->connections();
  }

  auto managed_node_graph::connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->connections(node);
  }

  auto managed_node_graph::connections(const socket_handle& socket) const
    -> std::vector<connection_handle>
  {
    if (!exists(socket))
      return {};

    return m_pimpl->connections(socket);
  }

  auto managed_node_graph::input_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->input_connections(node);
  }

  auto managed_node_graph::output_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->output_connections(node);
  }

  auto managed_node_graph::get_data(const socket_handle& socket) const
    -> object_ptr<Object>
  {
    if (!exists(socket))
      return {};

    return m_pimpl->get_data(socket);
  }

  void managed_node_graph::set_data(
    const socket_handle& socket,
    object_ptr<Object> data)
  {
    if (!exists(socket))
      return;

    return m_pimpl->set_data(socket, data);
  }

  void managed_node_graph::clear()
  {
    m_pimpl->clear();
  }

  auto managed_node_graph::node_graph() const -> const yave::node_graph&
  {
    return m_pimpl->ng;
  }

  managed_node_graph::managed_node_graph(std::unique_ptr<impl>&& pimpl) noexcept
    : m_pimpl {std::move(pimpl)}
  {
  }

  auto managed_node_graph::clone() const -> managed_node_graph
  {
    return managed_node_graph(std::make_unique<impl>(m_pimpl->clone()));
  }

} // namespace yave

YAVE_DECL_TYPE(yave::NodeData, "1e725a89-3112-4698-bc3f-234f1f426cc6");
YAVE_DECL_TYPE(yave::SocketData, "f0113409-a700-4a25-9d18-24ece5f4c28f");