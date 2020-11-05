//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/structured_node_graph.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/rts/value_cast.hpp>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <range/v3/action.hpp>

#include <yave/lib/util/variant_mixin.hpp>
#include <yave/support/log.hpp>
#include <yave/support/overloaded.hpp>
#include <yave/rts/box.hpp>

#include <string_view>
#include <algorithm>
#include <regex>

YAVE_DECL_G_LOGGER(structured_node_graph)

namespace yave {

  namespace {

    namespace rng = ranges;

    // inline assertion
    inline constexpr auto check = [](auto&& arg) {
      assert(arg);
      return std::forward<decltype(arg)>(arg);
    };

    // internal node tag types
    namespace node {
      /// Interface node which represents node group
      class NodeGroupInterface;
      /// Node dependency
      class NodeDependency;
      /// Interface node which represents node input
      class NodeGroupInput;
      /// Interface node which represents node output
      class NodeGroupOutput;
      /// Node group I/O socket node
      class NodeGroupIOBit;
    } // namespace node

    // internal node parameter
    auto get_declaration(meta_type<node::NodeGroupInterface>)
    {
      return composed_node_declaration(
        "_.NodeGroupInterface",
        "",
        node_declaration_visibility::_private,
        {},
        {},
        [](auto&, auto) {});
    }

    // internal node parameter
    auto get_declaration(meta_type<node::NodeDependency>)
    {
      return function_node_declaration(
        "_.NodeGroupDependency",
        "",
        node_declaration_visibility::_private,
        {""},
        {""});
    }

    // internal node parameter
    auto get_declaration(meta_type<node::NodeGroupIOBit>)
    {
      return function_node_declaration(
        "_.NodeGroupIOBit",
        "",
        node_declaration_visibility::_private,
        {""},  // will change dynamically
        {""}); // will change dynamically
    }

    // internal node parameter
    auto get_declaration(meta_type<node::NodeGroupInput>)
    {
      return function_node_declaration(
        "_.NodeGroupInput", "", node_declaration_visibility::_private, {}, {});
    }

    // internal node parameter
    auto get_declaration(meta_type<node::NodeGroupOutput>)
    {
      return function_node_declaration(
        "_.NodeGroupOutput", "", node_declaration_visibility::_private, {}, {});
    }

    // fwd
    struct node_group;
    struct node_function;
    struct node_macro;
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
      ///  declaration
      std::shared_ptr<composed_node_declaration> pdecl;
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
      /// properties
      std::map<std::string, object_ptr<Object>> properties;

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

      auto& io_bits(socket_type type);
      auto& io_handler(socket_type type);

      auto get_defcall();

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
      std::shared_ptr<function_node_declaration> pdecl;
      /// callers
      std::vector<node_call*> callers;
      /// properties
      std::map<std::string, object_ptr<Object>> properties;

      void refresh(const node_graph& ng);
    };

    /// node macro
    struct node_macro
    {
      /// macro body
      node_handle node;
      /// dependency
      node_handle dependency;
      /// decl
      std::shared_ptr<macro_node_declaration> pdecl;
      /// callers
      std::vector<node_call*> callers;
      /// properties
      std::map<std::string, object_ptr<Object>> properties;

      void refresh(const node_graph& ng);
    };

    /// callee type
    struct node_callee : variant_mixin<node_function*, node_group*, node_macro*>
    {
      using variant_mixin::variant_mixin;

      bool is_defcall(const node_call* caller) const
      {
        return visit([&](auto* p) {
          assert(!p->callers.empty());
          return p->callers.front() == caller;
        });
      }

      auto get_defcall() const
      {
        return visit([](auto* p) {
          assert(!p->callers.empty());
          return p->callers.front();
        });
      }

      void add_caller(node_call* caller)
      {
        visit([&](auto* p) {
          auto& callers = p->callers;

          assert(rng::find(callers, caller) == callers.end());
          // first caller becomes defcall
          callers.push_back(caller);
        });
      }

      void remove_caller(node_call* caller)
      {
        visit([&](auto* p) {
          auto& callers = p->callers;

          assert(!callers.empty());
          if (callers.size() > 1)
            assert(callers.front() != caller);

          assert(rng::find(callers, caller) != callers.end());
          callers.erase(rng::find(callers, caller));
        });
      }
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
      node_callee callee;
      /// input bits
      std::vector<node_handle> input_bits;
      /// output bits
      std::vector<node_handle> output_bits;
      /// pos
      glm::dvec2 pos = {};
      /// properties
      std::map<std::string, object_ptr<Object>> properties;

      bool is_global() const
      {
        return parent == nullptr;
      }

      bool is_defcall() const
      {
        return callee.is_defcall(this);
      }

      auto& io_bits(socket_type type);

      void refresh(const node_graph& ng);
    };

    // for io handlers
    struct node_io
    {
      enum class io_type
      {
        input,
        output,
      };
      /// parent group
      node_group* parent;
      /// type
      io_type type;
      /// pos
      glm::dvec2 pos = {};
      /// properties
      std::map<std::string, object_ptr<Object>> properties;

      bool is_input() const
      {
        return type == io_type::input;
      }

      bool is_output() const
      {
        return type == io_type::output;
      }

      void refresh(const node_graph& ng);
    };

    /// node data variant
    struct node_data
      : variant_mixin<node_function, node_group, node_macro, node_call, node_io>
    {
      void refresh(node_graph& ng)
      {
        visit([&](auto& x) { x.refresh(ng); });
      }

      auto get_property(const std::string& name) -> object_ptr<Object>
      {
        return visit([&](auto& x) -> object_ptr<Object> {
          auto& map = x.properties;

          if (auto it = map.find(name); it != map.end())
            return it->second;

          return nullptr;
        });
      }

      void set_property(const std::string& name, object_ptr<Object> obj)
      {
        visit([&](auto& x) {
          auto& map = x.properties;
          map.insert_or_assign(name, std::move(obj));
        });
      }

      void remove_property(const std::string& name)
      {
        visit([&](auto& x) { x.properties.erase(name); });
      }
    };

    /// internal socket data
    struct socket_data
    {
      std::map<std::string, object_ptr<Object>> properties;

      auto get_property(const std::string& name) -> object_ptr<Object>
      {
        if (auto it = properties.find(name); it != properties.end())
          return it->second;

        return nullptr;
      }

      void set_property(const std::string& name, object_ptr<Object> data)
      {
        properties.insert_or_assign(name, std::move(data));
      }

      void remove_peoperty(const std::string& name)
      {
        properties.erase(name);
      }

      void clone_properties()
      {
        for (auto&& [key, prop] : properties)
          prop = prop.clone();
      }
    };

    /// socket data object
    using SocketData = Box<socket_data>;
    /// node data object
    using NodeData = Box<node_data>;

    /// create new node data
    template <class Arg>
    [[nodiscard]] auto make_node_data(Arg&& arg)
    {
      return make_object<NodeData>(std::forward<Arg>(arg));
    }

    // io bits
    auto _get_io_bits_ref = [](auto* p, auto type) -> auto&
    {
      switch (type) {
        case socket_type::input:
          return p->input_bits;
        case socket_type::output:
          return p->output_bits;
      }
      unreachable();
    };

    auto _get_io_handler_ref = [](auto* p, auto type) -> auto&
    {
      switch (type) {
        case socket_type::input:
          return p->input_handler;
        case socket_type::output:
          return p->output_handler;
      }
      unreachable();
    };

    auto& node_group::io_bits(socket_type type)
    {
      return _get_io_bits_ref(this, type);
    }

    auto& node_group::io_handler(socket_type type)
    {
      return _get_io_handler_ref(this, type);
    }

    auto& node_call::io_bits(socket_type type)
    {
      return _get_io_bits_ref(this, type);
    }

    auto node_group::get_defcall()
    {
      return node_callee(this).get_defcall();
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
        caller     = &std::get<node_call>(*value_cast<NodeData>(cdata));
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
        caller     = &std::get<node_call>(*value_cast<NodeData>(cdata));
      }
    }

    void node_macro::refresh(const node_graph& ng)
    {
      auto map = [&](auto n) { return ng.node(n.id()); };

      node       = map(node);
      dependency = map(dependency);

      for (auto&& caller : callers) {
        auto c     = map(caller->node);
        auto cdata = ng.get_data(c);
        caller     = &std::get<node_call>(*value_cast<NodeData>(cdata));
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
      parent  = &std::get<node_group>(*value_cast<NodeData>(ng.get_data(pn)));

      callee.visit([&](auto& p) {
        using pt = std::decay_t<decltype(*p)>;
        p = &std::get<pt>(*value_cast<NodeData>(ng.get_data(map(p->node))));
      });
    }

    void node_io::refresh(const node_graph& ng)
    {
      auto pn = ng.node(parent->node.id());
      parent  = &std::get<node_group>(*value_cast<NodeData>(ng.get_data(pn)));
    }

    /// get default name of node group
    auto get_default_new_group_name(uid id)
    {
      return fmt::format("Group{}", to_string(id).substr(0, 4));
    }

  } // namespace

  class structured_node_graph::impl
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
      auto g = check(add_new_group());
      root   = g->node;
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

    impl(std::nullptr_t) noexcept
    {
    }

    impl(impl&&) noexcept = default;
    ~impl() noexcept      = default;

  private:
    auto get_data(const node_handle& node) const -> object_ptr<NodeData>
    {
      return value_cast<NodeData>(ng.get_data(node));
    }

    auto get_data(const socket_handle& socket) const -> object_ptr<SocketData>
    {
      return value_cast<SocketData>(ng.get_data(socket));
    }

    void set_data(const node_handle& node, object_ptr<NodeData> data)
    {
      assert(!ng.get_data(node));
      ng.set_data(node, std::move(data));
    }

    void set_data(const socket_handle& socket, object_ptr<SocketData> data)
    {
      assert(!ng.get_data(socket));
      ng.set_data(socket, std::move(data));
    }

  public:
    auto get_caller_property(const node_handle& n, const std::string& name)
      -> object_ptr<Object>
    {
      assert(get_call(n) || get_io(n));
      return get_data(n)->get_property(name);
    }

    auto get_caller_property(const socket_handle& s, const std::string& name)
    {
      assert(get_call(ng.interfaces(s)[0]) || get_io(ng.interfaces(s)[0]));
      return get_data(s)->get_property(name);
    }

    void set_caller_property(
      const node_handle& n,
      const std::string& name,
      object_ptr<Object> data)
    {
      assert(get_call(n) || get_io(n));
      get_data(n)->set_property(name, std::move(data));
    }

    void set_caller_property(
      const socket_handle& s,
      const std::string& name,
      object_ptr<Object> data)
    {
      assert(get_call(ng.interfaces(s)[0]) || get_io(ng.interfaces(s)[0]));
      get_data(s)->set_property(name, std::move(data));
    }

  private:
    auto get_index(const node_handle& node, const socket_handle& socket) const
      -> size_t
    {
      if (ng.is_input_socket(socket)) {
        auto ss = ng.input_sockets(node);
        return rng::distance(ss.begin(), rng::find(ss, socket));
      }
      if (ng.is_output_socket(socket)) {
        auto ss = ng.output_sockets(node);
        return rng::distance(ss.begin(), rng::find(ss, socket));
      }
      unreachable();
    }

  public:
    auto get_index(const socket_handle& socket) const
    {
      assert(ng.interfaces(socket).size() == 1);
      auto n = ng.interfaces(socket)[0];
      assert(is_valid(n));
      return get_index(n, socket);
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
            n = call->parent->get_defcall()->node;
          else
            break; // hit root
        }

        if (auto io = get_io(n))
          n = io->parent->get_defcall()->node;
      }

      auto path = queue                   //
                  | rng::views::reverse   //
                  | rng::views::join('.') //
                  | rng::to<std::string>;

      assert(std::regex_match(path, std::regex(path_name_regex)));

      return path;
    }

    auto search_path(const std::string& path) const -> std::vector<node_handle>
    {
      static const auto re = std::regex(path_search_regex);

      if (!std::regex_match(path, re)) {
        Error(g_logger, "Invalid path format: {}", path);
        return {};
      }

      node_group* g       = get_group(root);
      std::string_view sv = path;

      while (true) {

        if (sv == "") {
          return g->members //
                 | rng::views::filter([&](auto&& n) { return is_defcall(n); })
                 | rng::to_vector;
        }

        auto pos = sv.find_first_of('.');

        // find name
        if (pos == sv.npos) {
          return g->nodes //
                 | rng::views::filter([&](auto&& n) {
                     return is_defcall(n) && *ng.get_name(n) == sv;
                   })
                 | rng::to_vector;
        }

        auto name = sv.substr(0, pos);

        for (auto&& n : g->nodes) {
          if (is_defcall(n) && *ng.get_name(n) == name) {

            // non-group: invalid
            if (is_function_call(n)) {
              return {};
            }

            // group: set next group
            if (auto group = get_callee_group(n)) {
              sv = sv.substr(pos + 1, sv.npos);
              g  = group;
              goto _continue;
            }
            unreachable();
          }
        }
        return {};

      _continue:;
      }
      return {};
    }

  public:
    bool is_parent_of(const node_handle& parent, const node_handle& child) const
    {
      auto g = child;
      while ((g = get_parent_group(g))) {
        if (g == parent)
          return true;
      }
      return false;
    }

    bool is_child_of(const node_handle& child, const node_handle& parent) const
    {
      return is_parent_of(parent, child);
    }

  private:
    auto create_call_bit(const std::string& name, socket_type type)
    {
      auto bit_decl = get_declaration(type_c<node::NodeGroupIOBit>);
      auto bit =
        check(ng.add(bit_decl.node_name(), {name}, {name}, node_type::normal));

      if (type == socket_type::input) {
        auto s = ng.input_sockets(bit)[0];
        set_data(s, make_object<SocketData>());
      }
      if (type == socket_type::output) {
        auto s = ng.output_sockets(bit)[0];
        set_data(s, make_object<SocketData>());
      }
      return bit;
    }

    auto create_group_bit(const std::string& name, socket_type type)
    {
      auto bit_decl = get_declaration(type_c<node::NodeGroupIOBit>);
      auto bit =
        check(ng.add(bit_decl.node_name(), {name}, {name}, node_type::normal));

      if (type == socket_type::input) {
        auto s = ng.output_sockets(bit)[0];
        set_data(s, make_object<SocketData>());
      }
      if (type == socket_type::output) {
        auto s = ng.input_sockets(bit)[0];
        set_data(s, make_object<SocketData>());
      }
      return bit;
    }

  private:
    /// create new group callee
    auto add_new_callee(const std::shared_ptr<composed_node_declaration>& pdecl)
      -> node_group*
    {
      auto d_decl = get_declaration(type_c<node::NodeDependency>);
      auto i_decl = get_declaration(type_c<node::NodeGroupInput>);
      auto o_decl = get_declaration(type_c<node::NodeGroupOutput>);

      // dependency
      auto d = check(ng.add(
        d_decl.node_name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal));

      // group interface
      auto g = check(ng.add(pdecl->node_name(), {}, {}, node_type::interface));
      auto i = check(ng.add(i_decl.node_name(), {}, {}, node_type::interface));
      auto o = check(ng.add(o_decl.node_name(), {}, {}, node_type::interface));

      auto gdata = make_node_data(node_group {
        .node           = g,
        .dependency     = d,
        .pdecl          = pdecl,
        .input_handler  = i,
        .output_handler = o,
        .nodes          = {i, o}});

      auto pgdata = &gdata->get<node_group>();

      auto idata = make_node_data(
        node_io {.parent = pgdata, .type = node_io::io_type::input});

      auto odata = make_node_data(
        node_io {.parent = pgdata, .type = node_io::io_type::output});

      set_data(g, gdata);
      set_data(i, idata);
      set_data(o, odata);

      for (auto&& s : pdecl->input_sockets()) {
        auto bit = create_group_bit(s, socket_type::input);
        check(ng.attach_interface(g, ng.input_sockets(bit)[0]));
        check(ng.attach_interface(o, ng.output_sockets(bit)[0]));
        pgdata->input_bits.push_back(bit);
      }

      for (auto&& s : pdecl->output_sockets()) {
        auto bit = create_group_bit(s, socket_type::output);
        check(ng.attach_interface(g, ng.output_sockets(bit)[0]));
        check(ng.attach_interface(o, ng.input_sockets(bit)[0]));
        pgdata->output_bits.push_back(bit);
      }

      Info(
        g_logger,
        "Added new group: {}, id={}",
        *ng.get_name(g),
        to_string(g.id()));

      return pgdata;
    }

    /// helper to create empty node group callee
    auto add_new_group() -> node_group*
    {
      return add_new_callee(std::make_shared<composed_node_declaration>(
        get_declaration(type_c<node::NodeGroupInterface>)));
    }

    /// create new function callee
    auto add_new_callee(const std::shared_ptr<function_node_declaration>& pdecl)
      -> node_function*
    {
      auto d_decl = get_declaration(type_c<node::NodeDependency>);

      auto body = ng.add(
        pdecl->node_name(),
        pdecl->input_sockets(),
        pdecl->output_sockets(),
        node_type::normal);

      auto dep = ng.add(
        d_decl.node_name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal);

      assert(body && dep);
      assert(!ng.get_data(body));

      auto bdata = make_node_data(node_function {
        .node = body, .dependency = dep, .pdecl = pdecl, .callers = {}});

      set_data(body, bdata);

      Info(
        g_logger,
        "Added new function: name={}, id={}",
        *ng.get_name(body),
        to_string(body.id()));

      return &bdata->get<node_function>();
    }

    /// create new macro callee
    auto add_new_callee(const std::shared_ptr<macro_node_declaration>& pdecl)
      -> node_macro*
    {
      auto d_decl = get_declaration(type_c<node::NodeDependency>);

      auto body = ng.add(
        pdecl->node_name(),
        pdecl->input_sockets(),
        pdecl->output_sockets(),
        node_type::normal);

      auto dep = ng.add(
        d_decl.node_name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal);

      assert(body && dep);
      assert(!ng.get_data(body));

      auto bdata = make_node_data(
        node_macro {.node = body, .dependency = dep, .pdecl = pdecl});

      set_data(body, bdata);

      Info(
        g_logger,
        "Added new macro node: name={}, id={}",
        *ng.get_name(body),
        to_string(body.id()));

      return std::get_if<node_macro>(&*bdata);
    }

    void remove_callee(node_callee callee)
    {
      // dispatch
      std::visit([&](auto& p) { remove_callee(p); }, callee);
    }

    // remove node gruop which no longer has callers.
    void remove_callee(node_group* group)
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
        remove_call(get_call(n));

      assert(group->members.empty());

      for (auto&& bit : group->input_bits)
        ng.remove(bit);

      for (auto&& bit : group->output_bits)
        ng.remove(bit);

      ng.remove(group->input_handler);
      ng.remove(group->output_handler);
      ng.remove(group->dependency);
      ng.remove(group->node);
    }

    /// remove function which no longer has callers.
    void remove_callee(node_function* func)
    {
      assert(func);

      Info(g_logger, "Removing function: id={}", to_string(func->node.id()));

      // assume callers are already removed
      assert(func->callers.empty());

      ng.remove(func->dependency);
      ng.remove(func->node);
    }

    /// remove macro which no longer has callers.
    void remove_callee(node_macro* macro)
    {
      assert(macro);

      Info(g_logger, "Removing macro: id={}", to_string(macro->node.id()));

      // assume callers are already removed
      assert(macro->callers.empty());

      ng.remove(macro->dependency);
      ng.remove(macro->node);
    }

    /// create new node call in group.
    /// \param parent paretn group
    /// \param callee callee node function or group
    auto add_new_call(
      node_group* parent,
      node_callee callee,
      uid id = uid::random_generate()) -> node_call*
    {
      assert(parent);

      auto d_decl = get_declaration(type_c<node::NodeDependency>);

      auto dep = check(ng.add(
        d_decl.node_name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal));

      // check dependency (ignore when it's root call)
      auto valid = std::visit(
        [&](auto* p) {
          assert(p);
          // call -> parent group
          check(ng.connect(
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
        check(std::visit([&](auto p) { return ng.get_info(p->node); }, callee));

      // create interface
      auto n = ng.add(info->name(), {}, {}, node_type::interface, id);

      // invalid id
      if (!n) {
        ng.remove(dep);
        return nullptr;
      }

      // setup in/out sockets for the interface
      std::vector<node_handle> ibits;
      std::vector<node_handle> obits;

      ibits.reserve(info->input_sockets().size());
      obits.reserve(info->output_sockets().size());

      for (auto&& s : info->input_sockets()) {
        auto bit = create_call_bit(*ng.get_name(s), socket_type::input);
        check(ng.attach_interface(n, ng.input_sockets(bit)[0]));
        ibits.push_back(bit);
      }

      for (auto&& s : info->output_sockets()) {
        auto bit = create_call_bit(*ng.get_name(s), socket_type::output);
        check(ng.attach_interface(n, ng.output_sockets(bit)[0]));
        obits.push_back(bit);
      }

      // in-out dependency
      for (auto&& obit : obits)
        for (auto&& ibit : ibits)
          check(
            ng.connect(ng.output_sockets(ibit)[0], ng.input_sockets(obit)[0]));

      assert(!ng.get_data(n));

      auto ndata = make_node_data(node_call {
        .node        = n,
        .dependency  = dep,
        .parent      = parent,
        .callee      = callee,
        .input_bits  = ibits,
        .output_bits = obits});

      // set data
      set_data(n, ndata);

      // add to parent
      parent->add_member(n);

      auto newcall = &std::get<node_call>(*ndata);

      // set caller pointer
      callee.add_caller(newcall);

      // find name collision
      auto defcall_name_collides = [&](const auto& name) {
        // find other defcalls which has same name to new node
        auto ns = parent->nodes //
                  | rng::views::filter([&](auto&& n) {
                      return n != newcall->node && newcall->is_defcall();
                    })
                  | rng::views::filter(
                    [&](auto&& n) { return ng.get_name(n) == name; });

        return !ns.empty();
      };

      // get non-colliding name
      auto fix_defcall_name_collision = [&](const auto& name) {
        // escape string
        auto tmp   = name;
        auto count = 1;
        while (defcall_name_collides(tmp)) {
          tmp = fmt::format("{}{}", name, count);
          ++count;
        }
        return tmp;
      };

      // fix name collision
      std::visit(
        [&](auto& p) {
          auto name_fixed = fix_defcall_name_collision(info->name());
          ng.set_name(n, name_fixed);
          ng.set_name(p->node, name_fixed);
        },
        callee);

      // init call
      std::visit(
        overloaded {
          [&](node_function* f) {
            // set default arguments for function for each function call
            auto iss = ng.input_sockets(n);
            for (auto&& [idx, val] : f->pdecl->default_args())
              get_data(iss[idx])->set_property("data", val.clone());
          },
          [](auto) {}},
        callee);

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
    void remove_call(node_call* call)
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
            remove_call(get_call(caller));
      }

      // remove bits
      for (auto&& bit : call->input_bits)
        ng.remove(bit);
      for (auto&& bit : call->output_bits)
        ng.remove(bit);

      // remove from parent
      call->parent->remove_member(call->node);

      // remove from caller
      call->callee.remove_caller(call);

      auto callee = call->callee;

      // remove call
      ng.remove(call->dependency);
      ng.remove(call->node);

      // remove function or group when it's orphan
      std::visit(
        [&](auto* p) {
          if (p->callers.empty())
            remove_callee(p);
        },
        callee);
    }

    // copy node call
    auto copy_call(
      node_group* parent,
      node_call* call,
      uid id = uid::random_generate()) -> node_call*
    {
      assert(parent);

      // create new call
      auto newc = add_new_call(parent, call->callee, id);

      // invalid id
      if (!newc)
        return nullptr;

      assert(call->input_bits.size() == newc->input_bits.size());
      assert(call->output_bits.size() == newc->output_bits.size());

      // clone socket data

      auto iss = ng.input_sockets(call->node);
      for (auto&& [idx, s] : iss | rng::views::enumerate) {
        auto data = get_data(s).clone();
        data->clone_properties();
      }
      auto oss = ng.output_sockets(call->node);
      for (auto&& [idx, s] : oss | rng::views::enumerate) {
        auto data = get_data(s).clone();
        data->clone_properties();
      }

      return newc;
    }

    // clone node call
    auto clone_call(
      node_group* parent,
      node_call* call,
      uid id = uid::random_generate()) -> node_call*
    {
      return std::visit(
        [&](auto* p) { return clone_call(parent, call, p, id); }, call->callee);
    }

    // clone node function
    auto clone_call(
      node_group* parent,
      node_call* call,
      node_function* /*callee*/,
      uid id) -> node_call*
    {
      // fallback to copy
      return copy_call(parent, call, id);
    }

    // clone node macro
    auto clone_call(
      node_group* parent,
      node_call* call,
      node_macro* /*callee*/,
      uid id) -> node_call*
    {
      // fallback to copy
      return copy_call(parent, call, id);
    }

    // clone node group
    auto clone_call(
      node_group* parent,
      node_call* call,
      node_group* callee,
      uid id) -> node_call*
    {
      Info(g_logger, "Cloning node group {}", *ng.get_name(callee->node));

      // fallback to copy
      if (!call->is_defcall())
        return copy_call(parent, call, id);

      // map from src socket to new socket
      std::map<socket_handle, socket_handle> smap;

      // create empty group
      auto newg = add_new_group();

      // copy node names
      ng.set_name(newg->node, *ng.get_name(callee->node));
      ng.set_name(newg->input_handler, *ng.get_name(callee->input_handler));
      ng.set_name(newg->output_handler, *ng.get_name(callee->output_handler));

      // copy/clone members
      for (auto&& n : callee->members) {
        assert(is_valid(n));

        auto newn = clone_call(newg, get_call(n))->node;
        assert(is_valid(newn));

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

      // create new call
      auto newc = add_new_call(parent, newg, id);

      // closed loop
      if (!newc) {
        remove_callee(newg);
        return nullptr;
      }

      // copy sockets
      for (auto&& s : ng.input_sockets(call->node))
        check(add_input_socket(newc->node, *ng.get_name(s), size_t(-1)));

      for (auto&& s : ng.output_sockets(call->node))
        check(add_output_socket(newc->node, *ng.get_name(s), size_t(-1)));

      { // map sockets of IO handler
        auto oldis = ng.input_sockets(callee->output_handler);
        auto oldos = ng.output_sockets(callee->input_handler);
        auto newis = ng.input_sockets(newg->output_handler);
        auto newos = ng.output_sockets(newg->input_handler);

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
      for (auto&& n : callee->members) {
        for (auto&& c : ng.input_connections(n)) {
          auto info = ng.get_info(c);
          auto srcs = maps(info->src_socket());
          auto dsts = maps(info->dst_socket());
          check(ng.connect(srcs, dsts));
        }
      }

      return newc;
    }

  public:
    auto get_info(const node_handle& node) const
    {
      auto info = ng.get_info(node);
      assert(info);

      auto type = [&] {
        if (is_function_call(node))
          return structured_node_type::function;
        if (is_group_call(node))
          return structured_node_type::group;
        if (is_group_input(node))
          return structured_node_type::group_input;
        if (is_group_output(node))
          return structured_node_type::group_output;
        unreachable();
      }();

      auto call_type = structured_call_type::call;

      if (is_defcall(node))
        call_type = structured_call_type::definition;

      auto pos = get_pos(node);

      return structured_node_info(
        info->name(),
        info->input_sockets(),
        info->output_sockets(),
        type,
        call_type,
        pos);
    }

    auto get_info(const socket_handle& socket) const
    {
      auto info = check(ng.get_info(socket));
      assert(info->interfaces().size() == 1);

      auto n = info->interfaces()[0];
      assert(is_valid(n));

      auto cs = ng.connections(socket);

      return structured_socket_info(
        info->name(), info->type(), n, get_index(n, socket), cs);
    }

    auto get_info(const connection_handle& connection) const
    {
      auto info = check(ng.get_info(connection));
      assert(info->src_interfaces().size() == 1);
      assert(info->dst_interfaces().size() == 1);

      return structured_connection_info(
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

    auto get_function(const node_handle& node) const -> node_function*
    {
      assert(!get_call(node));
      assert(get_data(node));
      return std::get_if<node_function>(&*get_data(node));
    }

    auto get_macro(const node_handle& node) const -> node_macro*
    {
      assert(!get_call(node));
      assert(get_data(node));
      return std::get_if<node_macro>(&*get_data(node));
    }

    auto get_group(const node_handle& node) const -> node_group*
    {
      assert(!get_call(node));
      assert(get_data(node));
      return std::get_if<node_group>(&*get_data(node));
    }

    auto get_io(const node_handle& node) const -> node_io*
    {
      assert(get_data(node));
      return std::get_if<node_io>(&*get_data(node));
    }

    auto get_callee_function(const node_handle& node) const -> node_function*
    {
      if (auto call = get_call(node))
        if (auto pp = std::get_if<node_function*>(&call->callee))
          return *pp;
      return nullptr;
    }

    auto get_callee_macro(const node_handle& node) const -> node_macro*
    {
      if (auto call = get_call(node))
        if (auto pp = std::get_if<node_macro*>(&call->callee))
          return *pp;
      return nullptr;
    }

    auto get_callee_group(const node_handle& node) const -> node_group*
    {
      if (auto call = get_call(node))
        if (auto pp = std::get_if<node_group*>(&call->callee))
          return *pp;
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
      if (auto call = get_call(node))
        return call->is_defcall();
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

    bool is_group_call(const node_handle& node) const
    {
      return get_callee_group(node);
    }

    bool is_function_call(const node_handle& node) const
    {
      return get_callee_function(node);
    }

    bool is_macro_call(const node_handle& node) const
    {
      return get_callee_macro(node);
    }

    bool is_group_output(const node_handle& node) const
    {
      if (auto io = get_io(node))
        return io->is_output();
      return false;
    }

    bool is_group_input(const node_handle& node) const
    {
      if (auto io = get_io(node))
        return io->is_input();
      return false;
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
      assert(is_valid(node));

      constexpr auto to_nodes =
        rng::views::transform([](auto* p) { return p->node; }) | rng::to_vector;

      if (auto call = get_call(node))
        return std::visit(
          [&](auto* p) { return p->callers | to_nodes; }, call->callee);

      return {};
    }

  public: /* for member function */
    bool is_function_node(const node_handle& node) const
    {
      return is_function_call(node);
    }

    bool is_macro_node(const node_handle& node) const
    {
      return is_macro_call(node);
    }

    bool is_group_node(const node_handle& node) const
    {
      return is_group_call(node);
    }

    bool is_definition_node(const node_handle& node) const
    {
      return is_defcall(node);
    }

    bool is_call_node(const node_handle& node) const
    {
      return is_call(node);
    }

    bool is_group_output_node(const node_handle& node) const
    {
      return is_group_output(node);
    }

    bool is_group_input_node(const node_handle& node) const
    {
      return is_group_input(node);
    }

    bool is_group_member_node(const node_handle& node) const
    {
      return is_call(node);
    }

  public:
    auto get_definition(const node_handle& node) const -> node_handle
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        return call->callee.get_defcall()->node;

      return {};
    }

    auto get_calls(const node_handle& node) const -> std::vector<node_handle>
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        return std::visit(
          [](auto* p) {
            auto&& cs = p->callers;
            assert(!cs.empty());
            return rng::subrange(cs.begin() + 1, cs.end())
                   | rng::views::transform(
                     [](auto* call) { return call->node; })
                   | rng::to_vector;
          },
          call->callee);

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
          return call->parent->get_defcall()->node;

      if (auto io = get_io(node))
        return io->parent->get_defcall()->node;

      return {};
    }

  public:
    auto get_pos(const node_handle& node) const -> glm::dvec2
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        return call->pos;

      if (auto io = get_io(node))
        return io->pos;

      unreachable();
    }

    void set_pos(const node_handle& node, const glm::dvec2& newpos)
    {
      assert(is_valid(node));

      if (auto call = get_call(node)) {
        call->pos = newpos;
        return;
      }

      if (auto io = get_io(node)) {
        io->pos = newpos;
        return;
      }

      unreachable();
    }

  public:
    void set_name(const node_handle& node, const std::string& name)
    {
      assert(is_valid(node));

      static const auto re = std::regex(node_name_regex);

      if (!std::regex_match(name, re)) {
        Error(g_logger, "Invalid node name: {}", name);
        return;
      }

      auto call = get_call(node);

      if (!call) {
        Info(g_logger, "Cannot change name of non-call nodes");
        return;
      }

      // for legacy
      if (!call->is_defcall()) {
        Info(g_logger, "Cannot change name of node from non-definition call");
        return;
      }

      if (auto g = get_callee_group(node)) {
        // check uniqueness of group name
        for (auto&& n : g->get_defcall()->parent->nodes) {
          if (n == node || !is_defcall(n))
            continue;

          if (ng.get_name(n) == name) {
            Error(
              g_logger,
              "Cannot have multiple definitions with same name '{}' in a group",
              name);
            return;
          }
        }
        // set name to group
        ng.set_name(g->node, name);
        // update caller names
        for (auto&& caller : g->callers)
          ng.set_name(caller->node, name);
      }
    }

    void set_name(const socket_handle& socket, const std::string& name)
    {
      // call
      auto node = this->node(socket);
      auto idx  = socket_index(socket);

      assert(is_valid(node));

      static const auto re = std::regex(socket_name_regex);

      if (!std::regex_match(name, re)) {
        Error(g_logger, "Invalid socket name: {}", name);
        return;
      }

      //  io handler case
      if (auto io = get_io(node)) {
        if (ng.is_input_socket(socket)) {
          auto group = io->parent->callers[0]->node;
          set_name(ng.output_sockets(group)[idx], name);
        }
        if (ng.is_output_socket(socket)) {
          auto group = io->parent->callers[0]->node;
          set_name(ng.input_sockets(group)[idx], name);
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
      }
    }

  private:
    // helper function to insert new bit to caller side
    auto insert_node_call_bit(
      node_call* call,
      const std::string& name,
      socket_type type,
      size_t index)
    {
      auto& bits = call->io_bits(type);

      // index range shoud be valid
      assert(0 <= index && index <= bits.size());

      // get socket attached/detached to interface
      auto bit_outer_socket = [&](auto bit) {
        switch (type) {
          case socket_type::input:
            return ng.input_sockets(bit)[0];
          case socket_type::output:
            return ng.output_sockets(bit)[0];
        }
        unreachable();
      };

      // detach bits from call interface
      for (auto&& bit : bits)
        ng.detach_interface(call->node, bit_outer_socket(bit));

      // insert new bit
      auto newbit = create_call_bit(name, type);
      bits.insert(bits.begin() + index, newbit);

      // attach bits
      for (auto&& bit : bits)
        check(ng.attach_interface(call->node, bit_outer_socket(bit)));

      // add in-out dependency
      switch (type) {
        case socket_type::input:
          for (auto&& obit : call->output_bits)
            check(ng.connect(
              ng.output_sockets(newbit)[0], ng.input_sockets(obit)[0]));
          break;
        case socket_type::output:
          for (auto&& ibit : call->input_bits)
            check(ng.connect(
              ng.output_sockets(ibit)[0], ng.input_sockets(newbit)[0]));
          break;
        default:
          unreachable();
      }
    }

    // helper function to insert new bit for group callee
    auto insert_group_callee_bit(
      node_group* g,
      const std::string& name,
      socket_type type,
      size_t index)
    {
      auto& bits = g->io_bits(type);

      // index range shoud be valid
      assert(0 <= index && index <= bits.size());

      auto bit_inner_socket = [&](auto bit) {
        switch (type) {
          case socket_type::input:
            return ng.output_sockets(bit)[0];
          case socket_type::output:
            return ng.input_sockets(bit)[0];
        }
        unreachable();
      };

      auto bit_outer_socket = [&](auto bit) {
        switch (type) {
          case socket_type::input:
            return ng.input_sockets(bit)[0];
          case socket_type::output:
            return ng.output_sockets(bit)[0];
        }
        unreachable();
      };

      // detach interfaces
      for (auto&& bit : bits) {
        ng.detach_interface(g->io_handler(type), bit_inner_socket(bit));
        ng.detach_interface(g->node, bit_outer_socket(bit));
      }

      // insert new bit
      auto newbit = create_group_bit(name, type);
      bits.insert(bits.begin() + index, newbit);

      // attach interfaces
      for (auto&& bit : bits) {
        check(ng.attach_interface(g->io_handler(type), bit_inner_socket(bit)));
        check(ng.attach_interface(g->node, bit_outer_socket(bit)));
      }
    }

  public:
    auto add_input_socket(
      const node_handle& node,
      const std::string& socket,
      size_t index) -> socket_handle
    {
      assert(is_valid(node));

      // io (redirect to group)
      if (auto io = get_io(node)) {
        if (io->is_output()) {
          auto outer =
            add_output_socket(io->parent->get_defcall()->node, socket, index);
          return ng.input_sockets(node)[socket_index(outer)];
        }
        Error(g_logger, "Cannot add input socket to input handler");
        return {};
      }

      // clamp index to valid range
      index = std::clamp(index, size_t(0), ng.input_sockets(node).size());

      // macro
      if (get_callee_macro(node)) {
        auto c = check(get_call(node));
        insert_node_call_bit(c, socket, socket_type::input, index);
        return ng.input_sockets(node)[index];
      }

      // group
      if (auto g = get_callee_group(node)) {

        insert_group_callee_bit(g, socket, socket_type::input, index);

        for (auto&& caller : g->callers)
          insert_node_call_bit(caller, socket, socket_type::input, index);

        return ng.input_sockets(node)[index];
      }

      Error(g_logger, "Tried to add socket to non-group");
      return {};
    }

    auto add_output_socket(
      const node_handle& node,
      const std::string& socket,
      size_t index) -> socket_handle
    {
      assert(is_valid(node));

      // io (redirect to group)
      if (auto io = get_io(node)) {
        if (io->is_input()) {
          auto outer =
            add_input_socket(io->parent->get_defcall()->node, socket, index);
          return ng.output_sockets(node)[socket_index(outer)];
        }
        Error(g_logger, "Cannot output socket to output handler");
        return {};
      }

      // clamp index to valid range
      index = std::clamp(index, size_t(0), ng.output_sockets(node).size());

      // macro
      if (get_callee_macro(node)) {
        auto c = check(get_call(node));
        insert_node_call_bit(c, socket, socket_type::output, index);
        return ng.output_sockets(node)[index];
      }

      // group
      if (auto g = get_callee_group(node)) {

        insert_group_callee_bit(g, socket, socket_type::output, index);

        for (auto&& caller : g->callers)
          insert_node_call_bit(caller, socket, socket_type::output, index);

        return ng.output_sockets(node)[index];
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
      auto type = ng.get_info(socket)->type();

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

      // remove io bit from group or call
      auto rm_bit = [&](auto* p, auto type, auto idx) {
        auto& bits = p->io_bits(type);
        assert(idx < bits.size());
        ng.remove(bits[idx]);
        bits.erase(bits.begin() + idx);
      };

      // macro
      if (get_callee_macro(node)) {
        rm_bit(get_call(node), type, idx);
        return;
      }

      // group
      if (auto g = get_callee_group(node)) {
        // callee group
        rm_bit(g, type, idx);
        // caller
        for (auto&& caller : g->callers)
          rm_bit(caller, type, idx);
        return;
      }
      Error(g_logger, "Tried to remove socket of non-group, ignored.");
    }

  private:
    // prepare parent groups for creating new declaration.
    // returns callee group, or null when path is invalid.
    auto create_declaration_path(const std::string& path) -> node_group*
    {
      std::string_view sv = path;

      // current group
      auto* g = get_group(root);

      // list of created groups
      std::vector<node_call*> new_group_calls;

      while (true) {

        auto pos  = sv.find_first_of('.');
        auto name = sv.substr(0, pos);

        // check name of declaration
        if (pos == sv.npos) {

          for (auto&& n : g->nodes) {

            // name collision!
            if (is_defcall(n) && *ng.get_name(n) == name) {

              Error(
                g_logger,
                "Failed to create declaration: Node {} already exists",
                name);

              // cleanup
              for (auto&& c : new_group_calls)
                remove_call(c);

              return nullptr;
            }
          }

          // success
          return g;
        }

        node_group* nextg = nullptr;

        for (auto&& n : g->nodes) {

          // if group already exists, use it
          if (is_defcall(n) && *ng.get_name(n) == name) {

            // group?
            nextg = get_callee_group(n);

            // name collision with non group!
            if (!nextg) {

              Error(
                g_logger,
                "Failed to create declaration: Node {} is not group",
                name);

              // cleanup
              for (auto&& c : new_group_calls)
                remove_call(c);

              return nullptr;
            }
          }
        }

        // if group doesn't exist then create new one
        if (!nextg) {

          auto newg = add_new_group();
          ng.set_name(newg->input_handler, "In");
          ng.set_name(newg->output_handler, "Out");
          ng.set_name(newg->node, std::string(name));

          auto newc = add_new_call(g, newg);
          new_group_calls.push_back(newc);

          nextg = newg;
        }

        assert(nextg);

        sv = sv.substr(pos + 1, sv.npos);
        g  = nextg;
      }

      return nullptr;
    }

  public:
    auto create_declaration(
      const std::shared_ptr<node_declaration>& pdecl,
      structured_node_graph& owner) -> node_handle
    {
      return std::visit(
        [&](auto& decl) -> node_handle {
          Info(g_logger, "Creating new declaration: {}", decl.full_name());

          using t = std::decay_t<decltype(decl)>;

          if (auto p = create_declaration_path(decl.full_name())) {

            // create defcall
            auto func = check(add_new_callee(std::shared_ptr<t>(pdecl, &decl)));
            auto call = check(add_new_call(p, func));

            // call init callback for composed group
            if constexpr (std::is_same_v<t, composed_node_declaration>) {
              Info(g_logger, "init_composed()");
              decl.init_composed(owner, call->node);
            }

            return call->node;
          }
          return {};
        },
        *pdecl);
    }

    auto create_group(
      const node_handle& parent,
      const std::vector<node_handle>& nodes,
      const uid& id) -> node_handle
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
      auto newc = add_new_call(g, newg, id);

      // invalid id
      if (!newc) {
        remove_callee(newg);
        return {};
      }

      // set default name
      set_name(newc->node, get_default_new_group_name(newc->node.id()));

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
        auto news = check(add_input_socket(
          newc->node, *ng.get_name(info->dst_socket()), size_t(-1)));
        ng.disconnect(c);
        check(ng.connect(info->src_socket(), news));
        check(ng.connect(
          ng.output_sockets(newg->input_handler).back(), info->dst_socket()));
      }
      for (auto&& c : ocs) {
        auto info = ng.get_info(c);
        auto news = check(add_output_socket(
          newc->node, *ng.get_name(info->src_socket()), size_t(-1)));
        ng.disconnect(c);
        check(ng.connect(
          info->src_socket(), ng.input_sockets(newg->output_handler).back()));
        check(ng.connect(news, info->dst_socket()));
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
        check(ng.connect(
          ng.output_sockets(call->dependency)[0],
          ng.input_sockets(newg->dependency)[0]));
      }

      return newc->node;
    }

    auto create_copy(
      const node_handle& parent_group,
      const node_handle& src,
      const uid& id) -> node_handle
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

      if (auto call = get_call(src))
        if (auto copied = copy_call(g, call, id))
          return copied->node;

      return {};
    }

    auto create_clone(
      const node_handle& parent_group,
      const node_handle& src,
      const uid& id) -> node_handle
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

      if (auto call = get_call(src))
        if (auto cloned = clone_call(g, call, id))
          return cloned->node;

      return {};
    }

    void destroy(const node_handle& node)
    {
      assert(is_valid(node));

      if (auto call = get_call(node))
        return remove_call(call);

      Error(g_logger, "This node cannot be removed");
    }

    auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket,
      const uid& id) -> connection_handle
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

      auto c  = ng.connect(src_socket, dst_socket, id);
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

    auto clone() const
    {
      Info(
        g_logger,
        "clone(): n={}, s={}, c={}",
        ng.nodes().size(),
        ng.sockets().size(),
        ng.connections().size());

      impl ret(nullptr);

      // clone node graph
      ret.ng   = ng.clone();
      ret.root = ret.ng.node(root.id());

      for (auto&& n : ret.ng.nodes()) {
        // clone node data
        if (auto data = ret.ng.get_data(n))
          ret.ng.set_data(n, data.clone());
        // clone socket data
        for (auto&& s : ret.ng.input_sockets(n))
          if (auto data = ret.ng.get_data(s))
            ret.ng.set_data(s, data.clone());
        for (auto&& s : ret.ng.output_sockets(n))
          if (auto data = ret.ng.get_data(s))
            ret.ng.set_data(s, data.clone());
      }

      // update handles and links
      for (auto&& n : ret.ng.nodes())
        if (auto data = ret.ng.get_data(n))
          value_cast<NodeData>(data)->refresh(ret.ng);

      return structured_node_graph(std::make_unique<impl>(std::move(ret)));
    }

    void clear()
    {
      ng.clear();
      init();
    }
  };

  structured_node_graph::structured_node_graph()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  structured_node_graph::structured_node_graph(std::unique_ptr<impl>&& pimpl)
    : m_pimpl {std::move(pimpl)}
  {
  }

  structured_node_graph::structured_node_graph(
    structured_node_graph&& other) noexcept
    : structured_node_graph()
  {
    std::swap(m_pimpl, other.m_pimpl);
  }

  structured_node_graph::~structured_node_graph() noexcept = default;

  structured_node_graph& structured_node_graph::operator=(
    structured_node_graph&& other) noexcept
  {
    structured_node_graph tmp = std::move(other);
    std::swap(m_pimpl, tmp.m_pimpl);
    return *this;
  }

  bool structured_node_graph::exists(const node_handle& node) const
  {
    return m_pimpl->ng.exists(node);
  }

  bool structured_node_graph::exists(const connection_handle& connection) const
  {
    return m_pimpl->ng.exists(connection);
  }

  bool structured_node_graph::exists(const socket_handle& socket) const
  {
    return m_pimpl->ng.exists(socket);
  }

  auto structured_node_graph::node(const uid& id) const -> node_handle
  {
    return m_pimpl->ng.node(id);
  }

  auto structured_node_graph::socket(const uid& id) const -> socket_handle
  {
    return m_pimpl->ng.socket(id);
  }

  auto structured_node_graph::connection(const uid& id) const
    -> connection_handle
  {
    return m_pimpl->ng.connection(id);
  }

  auto structured_node_graph::get_info(const node_handle& node) const
    -> std::optional<structured_node_info>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_info(node);
  }

  auto structured_node_graph::get_info(const socket_handle& socket) const
    -> std::optional<structured_socket_info>
  {
    if (!exists(socket))
      return std::nullopt;

    return m_pimpl->get_info(socket);
  }

  auto structured_node_graph::get_info(const connection_handle& connection)
    const -> std::optional<structured_connection_info>
  {
    if (!exists(connection))
      return std::nullopt;

    return m_pimpl->get_info(connection);
  }

  auto structured_node_graph::get_name(const node_handle& node) const
    -> std::optional<std::string>
  {
    return m_pimpl->ng.get_name(node);
  }

  auto structured_node_graph::get_name(const socket_handle& socket) const
    -> std::optional<std::string>
  {
    return m_pimpl->ng.get_name(socket);
  }

  void structured_node_graph::set_name(
    const node_handle& node,
    const std::string& name)
  {
    if (!exists(node))
      return;

    m_pimpl->set_name(node, name);
  }

  void structured_node_graph::set_name(
    const socket_handle& socket,
    const std::string& name)
  {
    if (!exists(socket))
      return;

    m_pimpl->set_name(socket, name);
  }

  auto structured_node_graph::get_pos(const node_handle& node) const
    -> std::optional<glm::dvec2>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_pos(node);
  }

  void structured_node_graph::set_pos(
    const node_handle& node,
    const glm::dvec2& newpos)
  {
    if (!exists(node))
      return;

    return m_pimpl->set_pos(node, newpos);
  }

  auto structured_node_graph::get_data(const socket_handle& socket) const
    -> object_ptr<Object>
  {
    if (!exists(socket))
      return {};

    return m_pimpl->get_caller_property(socket, "data");
  }

  void structured_node_graph::set_data(
    const socket_handle& socket,
    object_ptr<Object> data)
  {
    if (!exists(socket))
      return;

    m_pimpl->set_caller_property(socket, "data", std::move(data));
  }

  auto structured_node_graph::_get_property(
    const node_handle& h,
    const std::string& name) -> object_ptr<Object>
  {
    if (!exists(h))
      return nullptr;

    return m_pimpl->get_caller_property(h, name);
  }

  auto structured_node_graph::_get_property(
    const socket_handle& h,
    const std::string& name) -> object_ptr<Object>
  {
    if (!exists(h))
      return nullptr;

    return m_pimpl->get_caller_property(h, name);
  }

  void structured_node_graph::set_property(
    const node_handle& h,
    const std::string& name,
    object_ptr<Object> data)
  {
    if (!exists(h))
      return;

    m_pimpl->set_caller_property(h, name, std::move(data));
  }

  void structured_node_graph::set_property(
    const socket_handle& h,
    const std::string& name,
    object_ptr<Object> data)
  {
    if (!exists(h))
      return;

    m_pimpl->set_caller_property(h, name, std::move(data));
  }

  auto structured_node_graph::get_index(const socket_handle& socket) const
    -> std::optional<size_t>
  {
    if (!exists(socket))
      return std::nullopt;

    return m_pimpl->get_index(socket);
  }

  auto structured_node_graph::get_path(const node_handle& node) const
    -> std::optional<std::string>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_path(node);
  }

  auto structured_node_graph::search_path(const std::string& path) const
    -> std::vector<node_handle>
  {
    return m_pimpl->search_path(path);
  }

  bool structured_node_graph::is_parent_of(
    const node_handle& parent,
    const node_handle& child) const
  {
    if (!exists(parent) || !exists(child))
      return false;

    return m_pimpl->is_parent_of(parent, child);
  }

  bool structured_node_graph::is_child_of(
    const node_handle& child,
    const node_handle& parent) const
  {
    if (!exists(child) || !exists(parent))
      return false;

    return m_pimpl->is_child_of(child, parent);
  }

  auto structured_node_graph::node(const socket_handle& socket) const
    -> node_handle
  {
    if (!exists(socket))
      return {};

    return m_pimpl->node(socket);
  }

  auto structured_node_graph::input_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return m_pimpl->ng.input_sockets(node);
  }

  auto structured_node_graph::output_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return m_pimpl->ng.output_sockets(node);
  }

  auto structured_node_graph::input_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_pimpl->ng.input_connections(node);
  }

  auto structured_node_graph::output_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_pimpl->ng.output_connections(node);
  }

  auto structured_node_graph::connections(const socket_handle& socket) const
    -> std::vector<connection_handle>
  {
    return m_pimpl->ng.connections(socket);
  }

  bool structured_node_graph::is_definition(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_definition_node(node);
  }

  auto structured_node_graph::get_definition(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_definition(node);
  }

  bool structured_node_graph::is_call(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_call_node(node);
  }

  auto structured_node_graph::get_calls(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_calls(node);
  }

  bool structured_node_graph::is_function(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_function_node(node);
  }

  bool structured_node_graph::is_macro(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_macro_node(node);
  }

  bool structured_node_graph::is_group(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_node(node);
  }

  bool structured_node_graph::is_group_member(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_member_node(node);
  }

  bool structured_node_graph::is_group_output(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_output_node(node);
  }

  bool structured_node_graph::is_group_input(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_input_node(node);
  }

  auto structured_node_graph::get_group_members(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_members(node);
  }

  auto structured_node_graph::get_group_input(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_input(node);
  }

  auto structured_node_graph::get_group_output(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_output(node);
  }

  auto structured_node_graph::get_group_nodes(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_group_nodes(node);
  }

  auto structured_node_graph::get_parent_group(const node_handle& node) const
    -> node_handle
  {
    if (!exists(node))
      return {};

    return m_pimpl->get_parent_group(node);
  }

  auto structured_node_graph::add_input_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index) -> socket_handle
  {
    if (!exists(group))
      return {};

    return m_pimpl->add_input_socket(group, socket, index);
  }

  auto structured_node_graph::add_output_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index) -> socket_handle
  {
    if (!exists(group))
      return {};

    return m_pimpl->add_output_socket(group, socket, index);
  }

  void structured_node_graph::remove_socket(const socket_handle& socket)
  {
    if (!exists(socket))
      return;

    return m_pimpl->remove_socket(socket);
  }

  void structured_node_graph::bring_front(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->bring_front(node);
  }

  void structured_node_graph::bring_back(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->bring_back(node);
  }

  auto structured_node_graph::create_declaration(
    const std::shared_ptr<node_declaration>& decl) -> node_handle
  {
    return m_pimpl->create_declaration(decl, *this);
  }

  auto structured_node_graph::create_group(
    const node_handle& parent_group,
    const std::vector<node_handle>& nodes,
    const uid& id) -> node_handle
  {
    if (parent_group && !exists(parent_group))
      return {};

    for (auto&& n : nodes)
      if (!exists(n))
        return {};

    return m_pimpl->create_group(parent_group, nodes, id);
  }

  auto structured_node_graph::create_copy(
    const node_handle& parent_group,
    const node_handle& src,
    const uid& id) -> node_handle
  {
    if (parent_group && !exists(parent_group))
      return {};

    if (!exists(src))
      return {};

    return m_pimpl->create_copy(parent_group, src, id);
  }

  auto structured_node_graph::create_clone(
    const node_handle& parent_group,
    const node_handle& src,
    const uid& id) -> node_handle
  {
    if (parent_group && !exists(parent_group))
      return {};

    if (!exists(src))
      return {};

    return m_pimpl->create_clone(parent_group, src, id);
  }

  void structured_node_graph::destroy(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->destroy(node);
  }

  auto structured_node_graph::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket,
    const uid& id) -> connection_handle
  {
    if (!exists(src_socket) || !exists(dst_socket))
      return {};

    return m_pimpl->connect(src_socket, dst_socket, id);
  }

  void structured_node_graph::disconnect(const connection_handle& c)
  {
    if (!exists(c))
      return;

    return m_pimpl->disconnect(c);
  }

  auto structured_node_graph::clone() const -> structured_node_graph
  {
    return m_pimpl->clone();
  }

  void structured_node_graph::clear()
  {
    m_pimpl->clear();
  }

} // namespace yave

YAVE_DECL_TYPE(yave::NodeData, "14834d06-dfeb-4a01-81d8-a2fe59a755c2");
YAVE_DECL_TYPE(yave::SocketData, "e22a82ce-1b16-481e-9702-087ff13217c8");