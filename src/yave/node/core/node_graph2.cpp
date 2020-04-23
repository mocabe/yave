//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_graph2.hpp>
#include <yave/node/core/node_group.hpp>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <range/v3/action.hpp>

#include <yave/support/log.hpp>
#include <yave/support/overloaded.hpp>
#include <yave/rts/box.hpp>

#include <variant>
#include <string_view>

YAVE_DECL_G_LOGGER(node_graph2)

namespace yave {

  namespace rng = ranges;

  // inline assertion
  constexpr auto expect = []([[maybe_unused]] auto&& arg) { assert(arg); };

  // fwd
  struct node_group;
  struct node_function;
  struct node_call;
  struct node_io;
  struct node_dep;

  /// node group
  struct node_group
  {
    /// interface node handle
    node_handle node;
    /// dependency node handle
    node_handle dependency;
    /// members in this group
    std::vector<node_handle> members;
    /// input handler node
    node_handle input_handler;
    /// output handler node
    node_handle output_handler;
    /// members + io handlers (for ordering nodes including io)
    std::vector<node_handle> nodes;
    /// input bits
    std::vector<node_handle> input_bits;
    /// output bits
    std::vector<node_handle> output_bits;
    /// callers
    std::vector<node_call*> callers;

    bool has_member(const node_handle& n)
    {
      return rng::find(members, n) != members.end();
    }

    void add_member(const node_handle& n)
    {
      assert(!has_member(n));
      assert(members.size() + 2 == nodes.size());
      members.push_back(n);
      nodes.push_back(n);
    }

    void remove_member(const node_handle& n)
    {
      assert(has_member(n));
      assert(members.size() + 2 == nodes.size());
      members.erase(rng::find(members, n));
      nodes.erase(rng::find(nodes, n));
    }

    void bring_front(const node_handle& n)
    {
      assert(has_member(n) || n == input_handler || n == output_handler);
      assert(members.size() + 2 == nodes.size());

      auto _bring_front = [](auto&& ns, auto&& n) {
        auto it = rng::remove(ns, n);
        if (it != ns.end()) {
          ns.erase(it, ns.end());
          ns.insert(ns.end(), n);
        }
      };

      _bring_front(members, n);
      _bring_front(nodes, n);
    }

    void bring_back(const node_handle& n)
    {
      assert(has_member(n) || n == input_handler || n == output_handler);
      assert(members.size() + 2 == nodes.size());

      auto _bring_back = [](auto&& ns, auto&& n) {
        auto it = rng::remove(ns, n);
        if (it != ns.end()) {
          ns.erase(it, ns.end());
          ns.insert(ns.begin(), n);
        }
      };

      _bring_back(members, n);
      _bring_back(nodes, n);
    }

    bool is_defcall(node_call* caller)
    {
      assert(!callers.empty());
      return callers.front() == caller;
    }

    auto defcall() const
    {
      assert(!callers.empty());
      return callers.front();
    }

    void add_caller(node_call* caller)
    {
      assert(rng::find(callers, caller) == callers.end());
      // first caller becomes defcall
      callers.push_back(caller);
    }

    void remove_caller(node_call* caller)
    {
      assert(!callers.empty());
      if (callers.size() > 1)
        assert(callers.front() != caller);

      assert(rng::find(callers, caller) != callers.end());
      callers.erase(rng::find(callers, caller));
    }

    void refresh(const node_graph& ng);
  };

  /// node function
  struct node_function
  {
    /// function body
    node_handle node;
    /// dependency node
    node_handle dependency;
    /// declaration
    node_declaration decl;
    /// callers
    std::vector<node_call*> callers;

    bool is_defcall(node_call* caller)
    {
      assert(!callers.empty());
      return callers.front() == caller;
    }

    auto defcall() const
    {
      assert(!callers.empty());
      return callers.front();
    }

    void add_caller(node_call* caller)
    {
      assert(rng::find(callers, caller) == callers.end());
      // first caller becomes defcall
      callers.push_back(caller);
    }

    void remove_caller(node_call* caller)
    {
      assert(!callers.empty());
      if (callers.size() > 1)
        assert(callers.front() != caller);

      assert(rng::find(callers, caller) != callers.end());
      callers.erase(rng::find(callers, caller));
    }

    void refresh(const node_graph& ng);
  };

  /// node call
  struct node_call
  {
    /// interface node
    node_handle node;
    /// depentency node
    node_handle dependency;
    /// parent group
    node_group* parent;
    /// handle of callee.
    /// either group or function.
    std::variant<node_function*, node_group*> callee;
    /// input bits
    std::vector<node_handle> input_bits;
    /// output bits
    std::vector<node_handle> output_bits;
    /// pos
    fvec2 pos = {};

    bool is_global() const
    {
      return parent == nullptr;
    }

    void refresh(const node_graph& ng);
  };

  // for io handlers
  struct node_io
  {
    /// parent group
    node_group* parent;
    /// type
    node_type2 type;
    /// pos
    fvec2 pos = {};

    void refresh(const node_graph& ng);
  };

  // for internal dependency
  struct node_dep
  {
    void refresh(const node_graph&);
  };

  /// node data variant
  using node_data =
    std::variant<node_function, node_group, node_call, node_io, node_dep>;

  /// custom node data
  using NodeData = Box<node_data>;

  /// create new node data
  template <class Arg>
  [[nodiscard]] auto make_node_data(Arg&& arg)
  {
    return make_object<NodeData>(std::forward<Arg>(arg));
  }

  // refresh functions

  void node_group::refresh(const node_graph& ng)
  {
    auto map = [&](auto n) { return ng.node(n.id()); };

    node           = map(node);
    dependency     = map(dependency);
    input_handler  = map(input_handler);
    output_handler = map(output_handler);

    for (auto&& n : members)
      n = map(n);
    for (auto&& n : input_bits)
      n = map(n);
    for (auto&& n : output_bits)
      n = map(n);
    for (auto&& n : nodes)
      n = map(n);

    for (auto&& caller : callers) {
      auto c     = map(caller->node);
      auto cdata = ng.get_data(c);
      caller = std::get_if<node_call>(value_cast_if<NodeData>(cdata).value());
      assert(caller);
    }
  }

  void node_function::refresh(const node_graph& ng)
  {
    auto map = [&](auto n) { return ng.node(n.id()); };

    node       = map(node);
    dependency = map(dependency);

    for (auto&& caller : callers) {
      auto c     = map(caller->node);
      auto cdata = ng.get_data(c);
      caller = std::get_if<node_call>(value_cast_if<NodeData>(cdata).value());
      assert(caller);
    }
  }

  void node_call::refresh(const node_graph& ng)
  {
    auto map = [&](auto n) { return ng.node(n.id()); };

    node       = map(node);
    dependency = map(dependency);

    for (auto&& n : input_bits)
      n = map(n);
    for (auto&& n : output_bits)
      n = map(n);

    auto pn = map(parent->node);
    parent =
      std::get_if<node_group>(value_cast_if<NodeData>(ng.get_data(pn)).value());
    assert(parent);

    std::visit(
      [&](auto*& p) {
        using tp = std::remove_pointer_t<std::decay_t<decltype(p)>>;
        p        = std::get_if<tp>(
          value_cast_if<NodeData>(ng.get_data(map(p->node))).value());
        assert(p);
      },
      callee);
  }

  void node_io::refresh(const node_graph& ng)
  {
    auto pn = ng.node(parent->node.id());
    parent =
      std::get_if<node_group>(value_cast_if<NodeData>(ng.get_data(pn)).value());
    assert(parent);
  }

  void node_dep::refresh(const node_graph&)
  {
  }

  /// We use two different DAG in internal representation. Group tree represents
  /// connections of node calls in group. Dependency tree represents `Node X
  /// belongs to group G` relations so that we can detect recursive call of
  /// node groups easily, which currently we do not allow to exist.
  class node_graph2::impl
  {
  public:
    /// graph
    yave::node_graph ng;
    /// internal root handle.
    /// this group should not have caller.
    node_handle root;

  public:
    void init()
    {
      assert(ng.empty());
      auto g = add_new_group();
      assert(g);
      root = g->node;
      // for readability in logs
      ng.set_name(g->node, "__root");
      ng.set_name(g->input_handler, "__in");
      ng.set_name(g->output_handler, "__out");
    }

    impl()
    {
      init_logger();
      init();
    }

    impl(impl&&) = default;

    ~impl() noexcept
    {
    }

  private:
    // get node data
    auto get_data(const node_handle& node) const -> object_ptr<NodeData>
    {
      if (auto data = ng.get_data(node)) {
        assert(has_type<NodeData>(data));
        return static_object_cast<NodeData>(data);
      }
      return nullptr;
    }
    // set node data
    void set_data(const node_handle& node, const object_ptr<NodeData>& data)
    {
      ng.set_data(node, data);
    }

  public:
    // get socket data
    auto get_data(const socket_handle& socket) const
    {
      assert(ng.exists(socket));
      return ng.get_data(socket);
    }

    // set socket data
    void set_data(const socket_handle& socket, object_ptr<Object> data)
    {
      assert(ng.exists(socket));

      if (auto d = ng.get_data(socket))
        if (!same_type(get_type(d), get_type(data)))
          Warning(
            g_logger,
            "Assigning different type of data to socket: id={}, t1={}, t2={}",
            to_string(socket.id()),
            to_string(get_type(d)),
            to_string(get_type(data)));

      ng.set_data(socket, std::move(data));
    }

  public:
    auto get_path(const node_handle& node) const -> std::optional<std::string>
    {
      assert(is_valid(node));

      node_handle n = node;
      std::vector<std::string> queue;

      while (true) {

        assert(is_valid(n));

        queue.push_back(*ng.get_name(n));

        if (auto call = get_call(n)) {
          if (call->parent != get_group(root))
            n = call->parent->defcall()->node;
          else
            break; // hit root
        }

        if (auto io = get_io(n))
          n = io->parent->defcall()->node;
      }

      std::string path;
      for (auto&& name : queue | rng::views::reverse)
        path += "/" + name;

      return path;
    }

    auto search_path(const std::string& path) const -> std::vector<node_handle>
    {
      node_group* g       = get_group(root);
      std::string_view sv = path;

      while (true) {

        if (sv == "/")
          return g->members //
                 | rng::views::filter(
                   [&](auto&& n) { return is_definition(n); })
                 | rng::to_vector;

        if (sv.find_first_of('/') != 0)
          return {};

        auto pos = sv.find_first_of('/', 1);

        if (pos == sv.npos) {

          auto name = sv.substr(1, pos);

          for (auto&& n : g->nodes)
            if (is_definition(n) && *ng.get_name(n) == name)
              return {n};

          return {};
        }

        auto name = sv.substr(1, pos - 1);

        for (auto&& n : g->nodes) {
          if (is_definition(n) && *ng.get_name(n) == name) {

            // non-group: invalid
            if (is_function_call(n)) {
              return {};
            }

            // group: set next group
            if (auto group = get_callee_group(n)) {
              sv.remove_prefix(pos);
              g = group;
              goto nextloop;
            }
            unreachable();
          }
        }
        return {};

      nextloop:;
      }
      return {};
    }

  private:
    /// create new group with empty I/O setting
    auto add_new_group() -> node_group*
    {
      auto g_decl = get_node_declaration<node::NodeGroupInterface>();
      auto i_decl = get_node_declaration<node::NodeGroupInput>();
      auto o_decl = get_node_declaration<node::NodeGroupOutput>();
      auto d_decl = get_node_declaration<node::NodeDependency>();

      auto g = ng.add(g_decl.name(), {}, {}, node_type::interface);
      auto i = ng.add(i_decl.name(), {}, {}, node_type::interface);
      auto o = ng.add(o_decl.name(), {}, {}, node_type::interface);

      auto d = ng.add(
        d_decl.name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal);

      auto gdata =
        make_node_data(node_group {g, d, {}, i, o, {i, o}, {}, {}, {}});
      auto idata = make_node_data(
        node_io {std::get_if<node_group>(&*gdata), node_type2::group_input});
      auto odata = make_node_data(
        node_io {std::get_if<node_group>(&*gdata), node_type2::group_output});
      auto ddata = make_node_data(node_dep());

      set_data(g, gdata);
      set_data(i, idata);
      set_data(o, odata);
      set_data(d, ddata);

      Info(
        g_logger, "Added new group: id={}", *ng.get_name(g), to_string(g.id()));

      return std::get_if<node_group>(&*gdata);
    }

    // remove node gruop which no longer has callers.
    void remove(node_group* group)
    {
      assert(group);

      Info(g_logger, "Removing node group: id={}", to_string(group->node.id()));

      for (auto&& m : group->members)
        Info(g_logger, "  {}", to_string(m.id()));

      // assume callers are already removed
      assert(group->callers.empty());

      // remove all members
      auto members = group->members;
      for (auto&& n : members)
        remove(get_call(n));

      assert(group->members.empty());

      // remove group

      for (auto&& bit : group->input_bits)
        ng.remove(bit);

      for (auto&& bit : group->output_bits)
        ng.remove(bit);

      ng.remove(group->input_handler);
      ng.remove(group->output_handler);
      ng.remove(group->dependency);
      ng.remove(group->node);
    }

    /// create new function decl
    auto add_new_function(const node_declaration& decl)
    {
      auto d_decl = get_node_declaration<node::NodeDependency>();

      auto body = ng.add(
        decl.name(),
        decl.input_sockets(),
        decl.output_sockets(),
        node_type::normal);

      auto dep = ng.add(
        d_decl.name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal);

      assert(!ng.get_data(body));

      auto bdata = make_node_data(node_function {body, dep, decl, {}});
      auto ddata = make_node_data(node_dep());

      ng.set_data(body, bdata);
      ng.set_data(dep, ddata);

      Info(
        g_logger,
        "Added new function: name={}, id={}",
        *ng.get_name(body),
        to_string(body.id()));

      return std::get_if<node_function>(&*bdata);
    }

    /// remove function which no longer has callers.
    void remove(node_function* func)
    {
      assert(func);

      Info(g_logger, "Removing function: id={}", to_string(func->node.id()));

      // assume callers are already removed
      assert(func->callers.empty());

      // remove function
      ng.remove(func->dependency);
      ng.remove(func->node);
    }

    auto create_node_call_bit(
      const node_handle& node,
      const std::string& name,
      socket_type type) -> node_handle
    {
      assert(ng.is_interface(node));
      auto bit_decl = get_node_declaration<node::NodeGroupIOBit>();

      // create bit
      auto bit = ng.add(bit_decl.name(), {name}, {name}, node_type::normal);

      if (type == socket_type::input)
        expect(ng.attach_interface(node, ng.input_sockets(bit)[0]));

      if (type == socket_type::output)
        expect(ng.attach_interface(node, ng.output_sockets(bit)[0]));

      return bit;
    }

    /// create new node call in group.
    /// \param parent paretn group
    /// \param callee callee node function or group
    auto add_new_call(
      node_group* parent,
      std::variant<node_function*, node_group*> callee) -> node_call*
    {
      assert(parent);

      auto d_decl = get_node_declaration<node::NodeDependency>();

      auto dep = ng.add(
        d_decl.name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal);

      // check dependency (ignore when it's root call)
      auto valid = std::visit(
        [&](auto* p) {
          // call -> parent group
          expect(ng.connect(
            ng.output_sockets(dep)[0],
            ng.input_sockets(parent->dependency)[0]));
          // caleee -> call
          return ng.connect(
            ng.output_sockets(p->dependency)[0], ng.input_sockets(dep)[0]);
        },
        callee);

      // closed loop
      if (!valid) {
        ng.remove(dep);
        Error(
          g_logger, "Failed to add node call: recursive call is not allowed");
        return nullptr;
      }

      // get info
      auto info =
        std::visit([&](auto p) { return ng.get_info(p->node); }, callee);

      assert(info);

      // create interface
      auto n = ng.add(info->name(), {}, {}, node_type::interface);

      // setup in/out sockets for the interface
      std::vector<node_handle> ibits;
      std::vector<node_handle> obits;

      ibits.reserve(info->input_sockets().size());
      obits.reserve(info->output_sockets().size());

      for (auto&& s : info->input_sockets())
        ibits.push_back(
          create_node_call_bit(n, *ng.get_name(s), socket_type::input));

      for (auto&& s : info->output_sockets())
        obits.push_back(
          create_node_call_bit(n, *ng.get_name(s), socket_type::output));

      // in-out dependency
      for (auto&& obit : obits)
        for (auto&& ibit : ibits)
          expect(
            ng.connect(ng.output_sockets(ibit)[0], ng.input_sockets(obit)[0]));

      assert(!ng.get_data(n));

      auto ndata =
        make_node_data(node_call {n, dep, parent, callee, ibits, obits});
      auto ddata = make_node_data(node_dep());

      set_data(n, ndata);
      set_data(dep, ddata);

      // add to parent
      parent->add_member(n);

      // set caller pointer
      std::visit(
        [&](auto* p) { p->add_caller(std::get_if<node_call>(&*ndata)); },
        callee);

      // set default arguments for function
      if (auto pf = std::get_if<node_function*>(&callee)) {
        auto f  = *pf;
        auto is = ng.input_sockets(n);
        for (auto&& [idx, val] : f->decl.default_arg()) {
          ng.set_data(is[idx], val.clone());
        }
      }

      Info(
        g_logger,
        "Added new call: name={}, id={}, def={}",
        *ng.get_name(n),
        to_string(n.id()),
        is_defcall(n));

      return std::get_if<node_call>(&*ndata);
    }

    // remove existing node call, also removes callee group or function when it
    // becomes orphan after removing the call. when the call is a defcall,
    // reomves all of callers too.
    void remove(node_call* call)
    {
      assert(call);
      assert(call->parent);
      assert(ng.exists(call->node));

      Info(g_logger, "Removing node call: id={}", to_string(call->node.id()));

      if (is_defcall(call->node)) {

        Info(g_logger, "Node {} is a defcall", to_string(call->node.id()));

        auto callers = get_caller_nodes(call->node);

        if (callers.size() > 1)
          Info(g_logger, "Following calls will also be removed: ");

        for (auto&& caller : callers)
          if (caller != call->node)
            Info(g_logger, "  {}", to_string(caller.id()));

        // remove normal calls before deleting defcall
        for (auto&& caller : callers)
          if (caller != call->node)
            remove(get_call(caller));
      }

      // remove bits
      for (auto&& bit : call->input_bits)
        ng.remove(bit);
      for (auto&& bit : call->output_bits)
        ng.remove(bit);

      // remove from parent
      call->parent->remove_member(call->node);

      // remove from caller
      std::visit([&](auto* p) { p->remove_caller(call); }, call->callee);

      auto callee = call->callee;

      // remove call
      ng.remove(call->dependency);
      ng.remove(call->node);

      // remove function or group when it's orphan
      std::visit(
        [&](auto* p) {
          if (p->callers.empty())
            remove(p);
        },
        callee);
    }

    auto copy_node_call(node_group* parent, node_call* call) -> node_call*
    {
      assert(parent);

      // create new call
      auto newc = add_new_call(parent, call->callee);

      if (!newc)
        return nullptr;

      assert(call->input_bits.size() == newc->input_bits.size());
      assert(call->output_bits.size() == newc->output_bits.size());

      // clone socket data

      auto iss = ng.input_sockets(call->node);
      for (auto&& [idx, s] : iss | rng::views::enumerate)
        if (auto data = get_data(s))
          set_data(ng.input_sockets(newc->node)[idx], data.clone());

      auto oss = ng.output_sockets(call->node);
      for (auto&& [idx, s] : oss | rng::views::enumerate)
        if (auto data = get_data(s))
          set_data(ng.output_sockets(newc->node)[idx], data.clone());

      return newc;
    }

    auto clone_node_group(node_group* parent, node_group* src) -> node_call*
    {
      assert(parent);
      assert(src);

      auto dep_decl = get_node_declaration<node::NodeDependency>();

      auto dep = ng.add(
        dep_decl.name(),
        dep_decl.input_sockets(),
        dep_decl.output_sockets(),
        node_type::normal);

      // check dependency
      expect(ng.connect(
        ng.output_sockets(dep)[0], ng.input_sockets(parent->dependency)[0]));

      if (!ng.connect(
            ng.output_sockets(src->dependency)[0], ng.input_sockets(dep)[0])) {
        ng.remove(dep);
        Error(
          g_logger,
          "Failed to clone node group: recursive call is not allowed");
        return nullptr;
      }

      // map from src socket to new socket
      std::map<socket_handle, socket_handle> smap;

      // create new group
      auto newg = add_new_group();
      ng.set_name(newg->input_handler, *ng.get_name(src->input_handler));
      ng.set_name(newg->output_handler, *ng.get_name(src->output_handler));

      // decide new name of node
      {
        auto name_used = [&](auto&& name) {
          for (auto&& n : parent->nodes)
            if (is_definition(n) && ng.get_name(n) == name)
              return true;
          return false;
        };

        auto name  = *ng.get_name(src->node);
        auto tmp   = name;
        auto count = 1;

        while (name_used(tmp)) {
          tmp = fmt::format("{}({})", name, count);
          ++count;
        }
        ng.set_name(newg->node, tmp);
      }

      // create new call
      auto newc = add_new_call(parent, newg);

      // copy sockets
      for (auto&& s : ng.input_sockets(src->node))
        expect(add_input_socket(newc->node, *ng.get_name(s), -1));

      for (auto&& s : ng.output_sockets(src->node))
        expect(add_output_socket(newc->node, *ng.get_name(s), -1));

      { // map sockets of IO handler
        auto oldis = ng.input_sockets(src->output_handler);
        auto oldos = ng.output_sockets(src->input_handler);
        auto newis = ng.input_sockets(newg->output_handler);
        auto newos = ng.output_sockets(newg->input_handler);

        assert(oldis.size() == newis.size());
        assert(oldos.size() == newos.size());

        for (size_t i = 0; i < oldis.size(); ++i)
          smap.emplace(oldis[i], newis[i]);
        for (size_t i = 0; i < oldos.size(); ++i)
          smap.emplace(oldos[i], newos[i]);
      }

      // copy node calls
      for (auto&& n : src->members) {

        auto newn = create_clone(newc->node, n);

        assert(ng.exists(newn));

        auto oldis = ng.input_sockets(n);
        auto oldos = ng.output_sockets(n);
        auto newis = ng.input_sockets(newn);
        auto newos = ng.output_sockets(newn);

        assert(oldis.size() == newis.size());
        assert(oldos.size() == newos.size());

        for (size_t i = 0; i < oldis.size(); ++i)
          smap.emplace(oldis[i], newis[i]);
        for (size_t i = 0; i < oldos.size(); ++i)
          smap.emplace(oldos[i], newos[i]);
      }

      auto maps = [&](auto&& s) {
        auto it = smap.find(s);
        assert(it != smap.end());
        assert(ng.exists(it->second));
        return it->second;
      };

      // copy connections
      for (auto&& n : src->members) {
        for (auto&& c : ng.input_connections(n)) {
          auto info = ng.get_info(c);
          auto srcs = maps(info->src_socket());
          auto dsts = maps(info->dst_socket());
          expect(ng.connect(srcs, dsts));
        }
      }

      return newc;
    }

  public:
    auto get_info(const node_handle& node) const
    {
      auto info = ng.get_info(node);
      assert(info);

      node_type2 type;

      if (is_function_call(node))
        type = node_type2::function;
      if (is_group_call(node))
        type = node_type2::group;
      if (is_group_input(node))
        type = node_type2::group_input;
      if (is_group_output(node))
        type = node_type2::group_output;

      node_call_type call_type = node_call_type::call;

      if (is_defcall(node))
        call_type = node_call_type::definition;

      auto pos = get_pos(node);

      return node_info2(
        info->name(),
        info->input_sockets(),
        info->output_sockets(),
        type,
        call_type,
        pos);
    }

    auto get_info(const socket_handle& socket) const
    {
      auto info = ng.get_info(socket);
      assert(info);
      assert(info->interfaces().size() == 1);

      return socket_info2(info->name(), info->type(), info->interfaces()[0]);
    }

    auto get_info(const connection_handle& connection) const
    {
      auto info = ng.get_info(connection);
      assert(info);
      assert(info->src_interfaces().size() == 1);
      assert(info->dst_interfaces().size() == 1);

      return connection_info2(
        info->src_interfaces()[0],
        info->src_socket(),
        info->dst_interfaces()[0],
        info->dst_socket());
    }

    auto node(const socket_handle& socket) const
    {
      assert(ng.interfaces(socket).size() == 1);
      return ng.interfaces(socket)[0];
    }

  private:
    auto socket_index(const socket_handle& socket) const -> size_t
    {
      assert(ng.exists(socket));

      auto node = this->node(socket);
      assert(is_valid(node));

      auto getidx = [&](auto&& ss) {
        size_t ret = rng::distance(ss.begin(), rng::find(ss, socket));
        assert(ret < ss.size());
        return ret;
      };

      if (ng.is_input_socket(socket))
        return getidx(ng.input_sockets(node));

      if (ng.is_output_socket(socket))
        return getidx(ng.output_sockets(node));

      unreachable();
    }

  private:
    auto get_call(const node_handle& node) const -> node_call*
    {
      assert(get_data(node));
      return std::get_if<node_call>(&*get_data(node));
    }
    auto get_group(const node_handle& node) const -> node_group*
    {
      assert(!get_call(node));
      assert(get_data(node));
      return std::get_if<node_group>(&*get_data(node));
    }
    auto get_function(const node_handle& node) const -> node_function*
    {
      assert(!get_call(node));
      assert(get_data(node));
      return std::get_if<node_function>(&*get_data(node));
    }
    auto get_io(const node_handle& node) const -> node_io*
    {
      assert(get_data(node));
      return std::get_if<node_io>(&*get_data(node));
    }
    auto get_dep(const node_handle& node) const -> node_dep*
    {
      assert(!get_call(node));
      assert(get_data(node));
      return std::get_if<node_dep>(&*get_data(node));
    }
    auto get_callee_group(const node_handle& node) const -> node_group*
    {
      if (auto call = get_call(node))
        if (auto pg = std::get_if<node_group*>(&call->callee))
          return *pg;
      return nullptr;
    }
    auto get_callee_function(const node_handle& node) const -> node_function*
    {
      if (auto call = get_call(node))
        if (auto pf = std::get_if<node_function*>(&call->callee))
          return *pf;
      return nullptr;
    }
    auto get_parent(const node_handle& node) const -> node_group*
    {
      if (auto call = get_call(node))
        return call->parent;

      if (auto io = get_io(node))
        return io->parent;

      return nullptr;
    }

  private:
    bool is_call(const node_handle& node) const
    {
      return get_call(node);
    }
    bool is_defcall(const node_handle& node) const
    {
      if (auto func = get_callee_function(node))
        if (func->is_defcall(get_call(node)))
          return true;
      if (auto group = get_callee_group(node))
        if (group->is_defcall(get_call(node)))
          return true;

      return false;
    }
    bool is_group(const node_handle& node) const
    {
      return get_group(node);
    }
    bool is_function(const node_handle& node) const
    {
      return get_function(node);
    }
    bool is_io(const node_handle& node) const
    {
      return get_io(node);
    }
    bool is_dep(const node_handle& node) const
    {
      return get_dep(node);
    }
    bool is_group_call(const node_handle& node) const
    {
      return get_callee_group(node);
    }
    bool is_function_call(const node_handle& node) const
    {
      return get_callee_function(node);
    }

  private:
    // for assertion
    [[maybe_unused]] bool is_valid(const node_handle& node) const
    {
      return ng.exists(node) && (is_call(node) || is_io(node));
    }

  private:
    // get callee node
    auto get_callee_node(const node_handle& node) const -> node_handle
    {
      assert(is_valid(node));
      if (auto cd = get_call(node))
        return std::visit(
          overloaded {[](auto* p) { return p->node; }}, cd->callee);
      return {};
    }

    // collect caller nodes, including defcall
    auto get_caller_nodes(const node_handle& node) const
      -> std::vector<node_handle>
    {
      auto callee = get_callee_node(node);

      constexpr auto to_nodes =
        rng::views::transform([](auto* p) { return p->node; }) | rng::to_vector;

      if (auto fd = get_function(callee))
        return fd->callers | to_nodes;

      if (auto gd = get_group(callee))
        return gd->callers | to_nodes;

      return {};
    }

  public:
    bool is_function_node(const node_handle& node) const
    {
      assert(is_valid(node));
      return is_function_call(node);
    }
    bool is_group_node(const node_handle& node) const
    {
      assert(is_valid(node));
      return is_group_call(node);
    }
    bool is_definition(const node_handle& node) const
    {
      assert(is_valid(node));
      return is_defcall(node);
    }
    bool is_group_output(const node_handle& node) const
    {
      assert(is_valid(node));

      if (auto io = get_io(node))
        return io->type == node_type2::group_output;

      return false;
    }
    bool is_group_input(const node_handle& node) const
    {
      assert(is_valid(node));

      if (auto io = get_io(node))
        return io->type == node_type2::group_input;

      return false;
    }
    bool is_group_member(const node_handle& node) const
    {
      assert(is_valid(node));
      return is_call(node);
    }

  public:
    auto get_definition(const node_handle& node) const -> node_handle
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        return std::visit(
          [](auto* p) { return p->defcall()->node; }, call->callee);

      return {};
    }

    auto get_group_members(const node_handle& node) const
      -> std::vector<node_handle>
    {
      assert(is_valid(node));

      if (auto g = get_callee_group(node))
        return g->members;

      return {};
    }

    auto get_group_input(const node_handle& node) const -> node_handle
    {
      assert(is_valid(node));

      if (auto g = get_callee_group(node))
        return g->input_handler;

      return {};
    }

    auto get_group_output(const node_handle& node) const -> node_handle
    {
      assert(is_valid(node));

      if (auto g = get_callee_group(node))
        return g->output_handler;

      return {};
    }

    auto get_group_nodes(const node_handle& node) const
      -> std::vector<node_handle>
    {
      assert(is_valid(node));

      if (auto g = get_callee_group(node))
        return g->nodes;

      return {};
    }

    auto get_parent_group(const node_handle& node) const -> node_handle
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        if (call->parent != get_group(root))
          return call->parent->defcall()->node;

      if (auto io = get_io(node))
        return io->parent->defcall()->node;

      return {};
    }

  public:
    auto get_pos(const node_handle& node) const -> fvec2
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        return call->pos;

      if (auto io = get_io(node))
        return io->pos;

      unreachable();
    }

    void set_pos(const node_handle& node, const fvec2& newpos)
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        call->pos = newpos;

      if (auto io = get_io(node))
        io->pos = newpos;

      unreachable();
    }

  public:
    void set_group_name(const node_handle& node, const std::string& name)
    {
      assert(is_valid(node));

      if (auto g = get_callee_group(node)) {
        // check uniqueness of group name
        for (auto&& n : g->defcall()->parent->nodes) {
          if (n == node || !is_definition(n))
            continue;

          if (ng.get_name(n) == name) {
            Error(
              g_logger,
              "Cannot have multiple definitions with same name in a group");
            return;
          }
        }
        // set name to group
        ng.set_name(g->node, name);
        // update caller names
        for (auto&& caller : g->callers)
          ng.set_name(caller->node, name);
      } else
        Error(g_logger, "Tried to rename non-group");
    }

    void set_socket_name(const socket_handle& socket, const std::string& name)
    {
      // call
      auto node = this->node(socket);
      auto idx  = socket_index(socket);

      assert(is_valid(node));

      //  io handler case
      if (auto io = get_io(node)) {
        if (ng.is_input_socket(socket)) {
          auto group = io->parent->callers[0]->node;
          return set_socket_name(ng.output_sockets(group)[idx], name);
        }
        if (ng.is_output_socket(socket)) {
          auto group = io->parent->callers[0]->node;
          return set_socket_name(ng.input_sockets(group)[idx], name);
        }
      }

      // change socket name of group
      if (auto g = get_callee_group(node)) {

        if (ng.is_input_socket(socket)) {
          // set group socket name
          ng.set_name(ng.input_sockets(g->input_bits[idx])[0], name);
          ng.set_name(ng.output_sockets(g->input_bits[idx])[0], name);

          for (auto&& caller : g->callers) {
            // set caller socket names
            ng.set_name(ng.input_sockets(caller->input_bits[idx])[0], name);
            ng.set_name(ng.output_sockets(caller->input_bits[idx])[0], name);
          }
        }

        if (ng.is_output_socket(socket)) {
          // set group socket name
          ng.set_name(ng.output_sockets(g->output_bits[idx])[0], name);
          ng.set_name(ng.input_sockets(g->output_bits[idx])[0], name);

          for (auto&& caller : g->callers) {
            // set caller socket names
            ng.set_name(ng.output_sockets(caller->output_bits[idx])[0], name);
            ng.set_name(ng.input_sockets(caller->output_bits[idx])[0], name);
          }
        }
      } else
        Error(g_logger, "Tried to change socket name of non-group");
    }

    auto add_input_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index) -> socket_handle
    {
      assert(is_valid(group));

      if (auto io = get_io(group)) {
        if (is_group_output(group)) {
          auto outer =
            add_output_socket(io->parent->defcall()->node, socket, index);
          return ng.input_sockets(group)[socket_index(outer)];
        }
        Error(g_logger, "Cannot add input socket to input handler");
        return {};
      }

      // add group input
      if (auto g = get_callee_group(group)) {

        if (index == size_t(-1))
          index = g->input_bits.size();

        // process group io bits
        {
          // detach bits
          for (auto&& bit : g->input_bits) {
            ng.detach_interface(g->input_handler, ng.output_sockets(bit)[0]);
            ng.detach_interface(g->node, ng.input_sockets(bit)[0]);
          }

          // insert new bit
          node_handle newbit;
          {
            auto decl = get_node_declaration<node::NodeGroupIOBit>();
            newbit = ng.add(decl.name(), {socket}, {socket}, node_type::normal);
            g->input_bits.insert(g->input_bits.begin() + index, newbit);
          }

          // attach bits
          for (auto&& bit : g->input_bits) {
            expect(
              ng.attach_interface(g->input_handler, ng.output_sockets(bit)[0]));
            expect(ng.attach_interface(g->node, ng.input_sockets(bit)[0]));
          }
        }

        // process each caller-size interfaces

        for (auto&& caller : g->callers) {
          // detach bit
          for (auto&& bit : caller->input_bits) {
            ng.detach_interface(caller->node, ng.input_sockets(bit)[0]);
          }
          // insert new bit
          node_handle newbit;
          {
            auto decl = get_node_declaration<node::NodeGroupIOBit>();
            newbit = ng.add(decl.name(), {socket}, {socket}, node_type::normal);
            caller->input_bits.insert(
              caller->input_bits.begin() + index, newbit);
          }
          // attach bits
          for (auto&& bit : caller->input_bits)
            expect(ng.attach_interface(caller->node, ng.input_sockets(bit)[0]));

          // add in-out connection
          for (auto&& obit : caller->output_bits)
            expect(ng.connect(
              ng.output_sockets(newbit)[0], ng.input_sockets(obit)[0]));
        }

        assert(ng.input_sockets(group).size() > index);
        return ng.input_sockets(group)[index];
      }

      Error(g_logger, "Tried to add socket to non-group");
      return {};
    }

    auto add_output_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index) -> socket_handle
    {
      assert(is_valid(group));

      if (auto io = get_io(group)) {
        if (is_group_input(group)) {
          auto outer =
            add_input_socket(io->parent->defcall()->node, socket, index);
          return ng.output_sockets(group)[socket_index(outer)];
        }
        Error(g_logger, "Cannot output socket to output handler");
        return {};
      }

      if (auto g = get_callee_group(group)) {

        if (index == size_t(-1))
          index = g->output_bits.size();

        // process group io bits
        {
          // detach bits
          for (auto&& bit : g->output_bits) {
            ng.detach_interface(g->output_handler, ng.input_sockets(bit)[0]);
            ng.detach_interface(g->node, ng.output_sockets(bit)[0]);
          }

          // insert new bit
          node_handle newbit;
          {
            auto decl = get_node_declaration<node::NodeGroupIOBit>();
            newbit = ng.add(decl.name(), {socket}, {socket}, node_type::normal);
            g->output_bits.insert(g->output_bits.begin() + index, newbit);
          }

          // attach bits
          for (auto&& bit : g->output_bits) {
            expect(
              ng.attach_interface(g->output_handler, ng.input_sockets(bit)[0]));
            expect(ng.attach_interface(g->node, ng.output_sockets(bit)[0]));
          }
        }

        // process each caller-size interfaces

        for (auto&& caller : g->callers) {
          // detach bit
          for (auto&& bit : caller->output_bits) {
            ng.detach_interface(caller->node, ng.output_sockets(bit)[0]);
          }
          // insert new bit
          node_handle newbit;
          {
            auto decl = get_node_declaration<node::NodeGroupIOBit>();
            newbit = ng.add(decl.name(), {socket}, {socket}, node_type::normal);
            caller->output_bits.insert(
              caller->output_bits.begin() + index, newbit);
          }
          // attach bits
          for (auto&& bit : caller->output_bits)
            expect(
              ng.attach_interface(caller->node, ng.output_sockets(bit)[0]));

          // add in-out connection
          for (auto&& ibit : caller->input_bits)
            expect(ng.connect(
              ng.output_sockets(ibit)[0], ng.input_sockets(newbit)[0]));
        }

        assert(ng.output_sockets(group).size() > index);
        return ng.output_sockets(group)[index];
      }

      Error(g_logger, "Tried to add socket to non-group");
      return {};
    }

    void remove_socket(const socket_handle& socket)
    {
      assert(ng.exists(socket));
      Info(g_logger, "Removing socket {}", to_string(socket.id()));

      auto node = this->node(socket);
      auto idx  = socket_index(socket);

      assert(is_valid(node));

      // io handler
      if (auto io = get_io(node)) {
        if (ng.is_input_socket(socket)) {
          auto group = io->parent->callers[0]->node;
          return remove_socket(ng.output_sockets(group)[idx]);
        }
        if (ng.is_output_socket(socket)) {
          auto group = io->parent->callers[0]->node;
          return remove_socket(ng.input_sockets(group)[idx]);
        }
      }

      if (auto g = get_callee_group(node)) {
        if (ng.is_input_socket(socket)) {
          auto rm_input = [&](auto* p, auto idx) {
            assert(idx < p->input_bits.size());
            ng.remove(p->input_bits[idx]);
            p->input_bits.erase(p->input_bits.begin() + idx);
          };
          // group
          rm_input(g, idx);
          // caller
          for (auto&& caller : g->callers)
            rm_input(caller, idx);
        }
        if (ng.is_output_socket(socket)) {
          auto rm_output = [&](auto* p, auto idx) {
            assert(idx < p->output_bits.size());
            ng.remove(p->output_bits[idx]);
            p->output_bits.erase(p->output_bits.begin() + idx);
          };
          // group
          rm_output(g, idx);
          // caller
          for (auto&& caller : g->callers)
            rm_output(caller, idx);
        }
      } else
        Error(g_logger, "Tried to remove socket of non-group, ignored.");
    }

  public:
    auto create_function(const node_declaration& decl) -> node_handle
    {
      auto path           = decl.qualified_name();
      std::string_view sv = path;

      Info(g_logger, "Creating new function: {}", path);

      // current group
      auto* g = get_group(root);

      while (true) {

        assert(sv.find_first_of('/') == 0);

        auto pos = sv.find_first_of('/', 1);

        // create function
        if (pos == sv.npos) {

          auto name = sv.substr(1, pos);
          assert(name == decl.name());

          for (auto&& n : g->nodes) {
            if (is_definition(n) && *ng.get_name(n) == name) {
              Error(
                g_logger,
                "Failed to create node function: Node {} already "
                "exists.",
                decl.qualified_name());
              return {};
            }
          }

          if (auto func = add_new_function(decl))
            if (auto call = add_new_call(g, func))
              return call->node;

          return {};
        }

        auto name = sv.substr(1, pos - 1);

        node_group* nextg = nullptr;

        // if group already exists, use it
        for (auto&& n : g->nodes) {
          if (is_definition(n) && *ng.get_name(n) == name) {
            // group?
            nextg = get_callee_group(n);
            // not group. abort
            if (!nextg) {
              Error(g_logger, "Failed to create function: Invalid path", name);
              return {};
            }
          }
        }

        // create new group
        if (!nextg) {
          auto newg = add_new_group();
          ng.set_name(newg->input_handler, "In");
          ng.set_name(newg->output_handler, "Out");
          auto newc = add_new_call(g, newg);
          set_group_name(newc->node, std::string(name));
          nextg = newg;
        }

        sv.remove_prefix(pos);
        g = nextg;
      }

      return {};
    }

    auto create_group(
      const node_handle& parent,
      const std::vector<node_handle>& nodes) -> node_handle
    {
      if (!parent && !nodes.empty()) {
        Error(g_logger, "Failed to gruop nodes: Cannot group global nodes");
        return {};
      }

      node_group* g = parent ? get_callee_group(parent) : get_group(root);

      // check parent
      if (!g) {
        Error(g_logger, "Failed to group nodes: Parent is not group");
        return {};
      }

      // check members
      for (auto&& n : nodes) {
        if (!g->has_member(n)) {
          Error(g_logger, "Failed to group nodes: Includes invalid node");
          return {};
        }
      }

      Info(g_logger, "Creating group: parent={}", to_string(parent.id()));
      Info(g_logger, "Following nodes will be moved into new group:");
      for (auto&& n : nodes)
        Info(g_logger, "  {}", to_string(n.id()));

      // create new group under parent
      auto newg = add_new_group();
      ng.set_name(newg->input_handler, "In");
      ng.set_name(newg->output_handler, "Out");

      // create new call
      auto newc = add_new_call(g, newg);
      set_group_name(
        newc->node,
        fmt::format("Group#{}", to_string(newc->node.id()).substr(0, 4)));

      // collect outbound connections
      std::vector<connection_handle> ocs, ics;

      auto addc = [&](auto& cs, auto& c) {
        if (rng::find(cs, c) == cs.end())
          cs.push_back(c);
      };

      for (auto&& n : nodes) {
        for (auto&& c : ng.input_connections(n)) {
          auto info = ng.get_info(c);
          assert(info->src_interfaces().size() == 1);
          if (rng::find(nodes, info->src_interfaces()[0]) == nodes.end())
            addc(ics, c);
        }
        for (auto&& c : ng.output_connections(n)) {
          auto info = ng.get_info(c);
          assert(info->dst_interfaces().size() == 1);
          if (rng::find(nodes, info->dst_interfaces()[0]) == nodes.end())
            addc(ocs, c);
        }
      }

      // Build new connections
      for (auto&& c : ics) {
        auto info = ng.get_info(c);
        auto news =
          add_input_socket(newc->node, *ng.get_name(info->dst_socket()), -1);
        assert(news);
        ng.disconnect(c);
        expect(ng.connect(info->src_socket(), news));
        expect(ng.connect(
          ng.output_sockets(newg->input_handler).back(), info->dst_socket()));
      }
      for (auto&& c : ocs) {
        auto info = ng.get_info(c);
        auto news =
          add_output_socket(newc->node, *ng.get_name(info->src_socket()), -1);
        assert(news);
        ng.disconnect(c);
        expect(ng.connect(
          info->src_socket(), ng.input_sockets(newg->output_handler).back()));
        expect(ng.connect(news, info->dst_socket()));
      }

      // move nodes to new group
      for (auto&& n : nodes) {

        assert(is_call(n));
        auto call = get_call(n);

        // move content
        g->remove_member(n);
        newg->add_member(n);
        call->parent = newg;

        // fix dependency
        assert(ng.output_connections(call->dependency).size() == 1);
        ng.disconnect(ng.output_connections(call->dependency)[0]);
        expect(ng.connect(
          ng.output_sockets(call->dependency)[0],
          ng.input_sockets(newg->dependency)[0]));
      }

      return newc->node;
    }

    auto create_copy(const node_handle& parent_group, const node_handle& src)
      -> node_handle
    {
      assert(is_valid(src));

      if (parent_group)
        assert(is_valid(parent_group));

      // when parent_group is null, use internal root
      node_group* g =
        parent_group ? get_callee_group(parent_group) : get_group(root);

      if (!g) {
        Error(g_logger, "Failed to copy node: Invalid parent group");
        return {};
      }

      if (auto c = get_call(src))
        if (auto newc = copy_node_call(g, c))
          return newc->node;

      // io cannot be copied

      Error(g_logger, "This node cannot be copied");
      return {};
    }

    auto create_clone(const node_handle& parent_group, const node_handle& src)
      -> node_handle
    {
      assert(is_valid(src));

      if (parent_group)
        assert(is_valid(parent_group));

      // when parent_group is null, use internal root
      node_group* g =
        parent_group ? get_callee_group(parent_group) : get_group(root);

      if (!g) {
        Error(g_logger, "Failed to clone node: Invalid parent group");
        return {};
      }

      // src is function call, fallback to copy
      if (get_callee_function(src))
        return create_copy(parent_group, src);

      // src is group call
      if (auto srcg = get_callee_group(src))
        if (auto newc = clone_node_group(g, srcg))
          return newc->node;

      // io cannot be cloned

      Error(g_logger, "This not cannot be cloned");
      return {};
    }

    void destroy(const node_handle& node)
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        return remove(call);

      Error(g_logger, "This not cannot be removed");
    }

    auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket) -> connection_handle
    {
      auto srcn = node(src_socket);
      auto dstn = node(dst_socket);

      assert(is_valid(srcn));
      assert(is_valid(dstn));

      auto srcp = get_parent(srcn);
      auto dstp = get_parent(dstn);

      if (srcp != dstp) {
        Error(g_logger, "Failed to connect: Not in same group");
        return {};
      }

      if (srcp == get_group(root)) {
        Error(g_logger, "Failed to connect: Global nodes cannot be connected");
        return {};
      }

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

    void disconnect(const connection_handle& c)
    {
      ng.disconnect(c);
    }

    void bring_front(const node_handle& node)
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        if (call->parent != get_group(root))
          call->parent->bring_front(node);

      if (auto io = get_io(node))
        io->parent->bring_front(node);
    }

    void bring_back(const node_handle& node)
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        if (call->parent != get_group(root))
          call->parent->bring_back(node);

      if (auto io = get_io(node))
        io->parent->bring_back(node);
    }

    auto clone() const -> impl
    {
      Info(
        g_logger,
        "clone(): n={}, s={}, c={}",
        ng.nodes().size(),
        ng.sockets().size(),
        ng.connections().size());

      impl ret;

      // clone node graph
      ret.ng   = ng.clone();
      ret.root = ret.ng.node(ret.root.id());

      // clone node data
      for (auto&& n : ret.ng.nodes())
        if (auto data = ret.get_data(n))
          ret.set_data(n, data.clone());

      // update handles and links
      for (auto&& n : ret.ng.nodes())
        if (auto data = ret.get_data(n))
          std::visit([&](auto& x) { x.refresh(ret.ng); }, *data);

      return ret;
    }

    void clear()
    {
      ng.clear();
      init();
    }
  };

  node_graph2::node_graph2()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  node_graph2::node_graph2(std::unique_ptr<impl>&& pimpl)
    : m_pimpl {std::move(pimpl)}
  {
  }

  node_graph2::node_graph2(node_graph2&&) noexcept = default;

  node_graph2::~node_graph2() noexcept = default;

  node_graph2& node_graph2::operator=(node_graph2&&) noexcept = default;

  bool node_graph2::exists(const node_handle& node) const
  {
    return m_pimpl->ng.exists(node);
  }

  bool node_graph2::exists(const connection_handle& connection) const
  {
    return m_pimpl->ng.exists(connection);
  }

  bool node_graph2::exists(const socket_handle& socket) const
  {
    return m_pimpl->ng.exists(socket);
  }

  auto node_graph2::node(const uid& id) const -> node_handle
  {
    return m_pimpl->ng.node(id);
  }

  auto node_graph2::socket(const uid& id) const -> socket_handle
  {
    return m_pimpl->ng.socket(id);
  }

  auto node_graph2::connection(const uid& id) const -> connection_handle
  {
    return m_pimpl->ng.connection(id);
  }

  auto node_graph2::get_info(const node_handle& node) const
    -> std::optional<node_info2>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_info(node);
  }

  auto node_graph2::get_info(const socket_handle& socket) const
    -> std::optional<socket_info2>
  {
    if (!exists(socket))
      return std::nullopt;

    return m_pimpl->get_info(socket);
  }

  auto node_graph2::get_info(const connection_handle& connection) const
    -> std::optional<connection_info2>
  {
    if (!exists(connection))
      return std::nullopt;

    return m_pimpl->get_info(connection);
  }

  auto node_graph2::get_name(const node_handle& node) const
    -> std::optional<std::string>
  {
    return m_pimpl->ng.get_name(node);
  }

  auto node_graph2::get_name(const socket_handle& socket) const
    -> std::optional<std::string>
  {
    return m_pimpl->ng.get_name(socket);
  }

  auto node_graph2::get_pos(const node_handle& node) const
    -> std::optional<fvec2>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_pos(node);
  }

  void node_graph2::set_pos(const node_handle& node, const fvec2& newpos)
  {
    if (!exists(node))
      return;

    return m_pimpl->set_pos(node, newpos);
  }

  auto node_graph2::get_data(const socket_handle& socket) const
    -> object_ptr<Object>
  {
    if (!exists(socket))
      return {};

    return m_pimpl->get_data(socket);
  }

  void node_graph2::set_data(
    const socket_handle& socket,
    object_ptr<Object> data)
  {
    if (!exists(socket))
      return;

    m_pimpl->set_data(socket, std::move(data));
  }

  auto node_graph2::get_path(const node_handle& node) const
    -> std::optional<std::string>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_path(node);
  }

  auto node_graph2::search_path(const std::string& path) const
    -> std::vector<node_handle>
  {
    return m_pimpl->search_path(path);
  }

  auto node_graph2::node(const socket_handle& socket) const -> node_handle
  {
    if (!exists(socket))
      return {};

    return m_pimpl->node(socket);
  }

  auto node_graph2::input_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return m_pimpl->ng.input_sockets(node);
  }

  auto node_graph2::output_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return m_pimpl->ng.output_sockets(node);
  }

  auto node_graph2::input_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_pimpl->ng.input_connections(node);
  }

  auto node_graph2::output_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_pimpl->ng.output_connections(node);
  }

  auto node_graph2::connections(const socket_handle& socket) const
    -> std::vector<connection_handle>
  {
    return m_pimpl->ng.connections(socket);
  }

  bool node_graph2::is_definition(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_definition(node);
  }

  auto node_graph2::get_definition(const node_handle& node) const -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_definition(node);
  }

  bool node_graph2::is_function(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_function_node(node);
  }

  bool node_graph2::is_group(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_node(node);
  }

  bool node_graph2::is_group_member(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_member(node);
  }

  bool node_graph2::is_group_output(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_output(node);
  }

  bool node_graph2::is_group_input(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_input(node);
  }

  auto node_graph2::get_group_members(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_members(node);
  }

  auto node_graph2::get_group_input(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_input(node);
  }

  auto node_graph2::get_group_output(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_output(node);
  }

  auto node_graph2::get_group_nodes(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_nodes(node);
  }

  auto node_graph2::get_parent_group(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_parent_group(node);
  }

  auto node_graph2::add_input_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index) -> socket_handle
  {
    if (!exists(group))
      return {};

    return m_pimpl->add_input_socket(group, socket, index);
  }

  auto node_graph2::add_output_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index) -> socket_handle
  {
    if (!exists(group))
      return {};

    return m_pimpl->add_output_socket(group, socket, index);
  }

  void node_graph2::remove_socket(const socket_handle& socket)
  {
    if (!exists(socket))
      return;

    return m_pimpl->remove_socket(socket);
  }

  void node_graph2::set_group_name(
    const node_handle& group,
    const std::string& name)
  {
    if (!exists(group))
      return;

    m_pimpl->set_group_name(group, name);
  }

  void node_graph2::set_socket_name(
    const socket_handle& socket,
    const std::string& name)
  {
    if (!exists(socket))
      return;

    m_pimpl->set_socket_name(socket, name);
  }

  void node_graph2::bring_front(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->bring_front(node);
  }

  void node_graph2::bring_back(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->bring_back(node);
  }

  auto node_graph2::create_function(const node_declaration& decl) -> node_handle
  {
    return m_pimpl->create_function(decl);
  }

  auto node_graph2::create_group(
    const node_handle& parent_group,
    const std::vector<node_handle>& nodes) -> node_handle
  {
    if (parent_group && !exists(parent_group))
      return {};

    for (auto&& n : nodes)
      if (!exists(n))
        return {};

    return m_pimpl->create_group(parent_group, nodes);
  }

  auto node_graph2::create_copy(
    const node_handle& parent_group,
    const node_handle& src) -> node_handle
  {
    if (parent_group && !exists(parent_group))
      return {};

    if (!exists(src))
      return {};

    return m_pimpl->create_copy(parent_group, src);
  }

  auto node_graph2::create_clone(
    const node_handle& parent_group,
    const node_handle& src) -> node_handle
  {
    if (parent_group && !exists(parent_group))
      return {};

    if (!exists(src))
      return {};

    return m_pimpl->create_clone(parent_group, src);
  }

  void node_graph2::destroy(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->destroy(node);
  }

  auto node_graph2::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket) -> connection_handle
  {
    if (!exists(src_socket) || !exists(dst_socket))
      return {};

    return m_pimpl->connect(src_socket, dst_socket);
  }

  void node_graph2::disconnect(const connection_handle& c)
  {
    if (!exists(c))
      return;

    return m_pimpl->disconnect(c);
  }

  auto node_graph2::clone() -> node_graph2
  {
    return node_graph2(std::make_unique<impl>(m_pimpl->clone()));
  }

  void node_graph2::clear()
  {
    m_pimpl->clear();
  }

} // namespace yave

YAVE_DECL_TYPE(yave::NodeData, "14834d06-dfeb-4a01-81d8-a2fe59a755c2");