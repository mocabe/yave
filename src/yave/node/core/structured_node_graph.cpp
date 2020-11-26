//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/structured_node_graph.hpp>
#include <yave/node/core/node_graph.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/rts/value_cast.hpp>
#include <yave/obj/vec/vec.hpp>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>

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

    namespace rn = ranges;
    namespace rv = ranges::views;

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
        [](auto&, auto) { return true; });
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
        return rn::find(members, n) != members.end();
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
        members.erase(rn::find(members, n));
        nodes.erase(rn::find(nodes, n));
      }

      void bring_front(const node_handle& n)
      {
        assert(has_member(n) || n == input_handler || n == output_handler);
        assert(members.size() + 2 == nodes.size());

        auto _bring_front = [](auto&& ns, auto&& n) {
          auto it = rn::remove(ns, n);
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
          auto it = rn::remove(ns, n);
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

          assert(rn::find(callers, caller) == callers.end());
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

          assert(rn::find(callers, caller) != callers.end());
          callers.erase(rn::find(callers, caller));
        });
      }

      auto& properties() const
      {
        return visit([](auto* p) -> auto& { return p->properties; });
      }

      auto get_property(const std::string& name) const -> object_ptr<Object>
      {
        return visit([&](auto* p) -> object_ptr<Object> {
          auto& map = p->properties;

          if (auto it = map.find(name); it != map.end())
            return it->second;

          return nullptr;
        });
      }

      void set_property(const std::string& name, object_ptr<Object> obj)
      {
        visit([&](auto* p) {
          auto& map = p->properties;
          map.insert_or_assign(name, std::move(obj));
        });
      }

      void remove_property(const std::string& name)
      {
        visit([&](auto* p) { p->properties.erase(name); });
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

      auto get_property(const std::string& name) const -> object_ptr<Object>
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

      auto get_property(const std::string& name) const -> object_ptr<Object>
      {
        if (auto it = properties.find(name); it != properties.end())
          return it->second;

        return nullptr;
      }

      void set_property(const std::string& name, object_ptr<Object> data)
      {
        properties.insert_or_assign(name, std::move(data));
      }

      void remove_property(const std::string& name)
      {
        properties.erase(name);
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

    using NodeDeclData = Box<std::shared_ptr<const node_declaration>>;

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
      auto g = check(add_new_callee_group());
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
    auto get_call(const node_handle& node) const -> node_call*
    {
      assert(get_data(node));
      return std::get_if<node_call>(&*get_data(node));
    }

    auto get_io(const node_handle& node) const -> node_io*
    {
      assert(get_data(node));
      return std::get_if<node_io>(&*get_data(node));
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
      if (get_call(node)) {
        throw;
      }
      assert(!get_call(node));
      assert(get_data(node));
      return std::get_if<node_group>(&*get_data(node));
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

  public:
    bool is_call(const node_handle& node) const
    {
      return get_call(node);
    }

    bool is_io(const node_handle& node) const
    {
      return get_io(node);
    }

    bool is_group(const node_handle& node) const
    {
      return get_group(node);
    }

    bool is_function(const node_handle& node) const
    {
      return get_function(node);
    }

    bool is_macro(const node_handle& node) const
    {
      return get_macro(node);
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

    bool is_group_member(const node_handle& node) const
    {
      return is_call(node);
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

    bool is_caller(const node_handle& node) const
    {
      return get_call(node) || get_io(node);
    }

    bool is_callee(const node_handle& node) const
    {
      return !is_caller(node);
    }

  public:
    // get node of socket
    auto get_node(const socket_handle& socket) const -> node_handle
    {
      assert(ng.interfaces(socket).size() == 1);
      return ng.interfaces(socket)[0];
    }

    // defcall?
    bool is_defcall(const node_handle& node) const
    {
      assert(is_caller(node));
      if (auto call = get_call(node))
        return call->callee.get_defcall() == call;
      return false;
    }

  private:
    // collect caller nodes, including defcall
    auto get_caller_nodes(const node_handle& node) const
      -> std::vector<node_handle>
    {
      assert(is_caller(node));

      constexpr auto to_nodes =
        rv::transform([](auto* p) { return p->node; }) | rn::to_vector;

      if (auto call = get_call(node))
        return call->callee.visit(
          [&](auto* p) { return p->callers | to_nodes; });

      return {};
    }

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
      const -> object_ptr<Object>
    {
      assert(is_caller(n));
      return get_data(n)->get_property(name);
    }

    auto get_caller_property(const socket_handle& s, const std::string& name)
      const
    {
      assert(is_caller(get_node(s)));
      return get_data(s)->get_property(name);
    }

    void set_caller_property(
      const node_handle& n,
      const std::string& name,
      object_ptr<Object> data)
    {
      assert(is_caller(n));
      get_data(n)->set_property(name, std::move(data));
    }

    void set_caller_property(
      const socket_handle& s,
      const std::string& name,
      object_ptr<Object> data)
    {
      assert(is_caller(get_node(s)));
      get_data(s)->set_property(name, std::move(data));
    }

    void remove_caller_property(const node_handle& n, const std::string& name)
    {
      assert(is_caller(n));
      get_data(n)->remove_property(name);
    }

    void remove_caller_property(const socket_handle& s, const std::string& name)
    {
      assert(is_caller(get_node(s)));
      get_data(s)->remove_property(name);
    }

  public:
    auto get_callee_property(const node_handle& n, const std::string& name)
      const -> object_ptr<Object>
    {
      assert(is_caller(n));
      if (auto call = get_call(n))
        return call->callee.get_property(name);
      return nullptr;
    }

    void set_callee_property(
      const node_handle& n,
      const std::string& name,
      object_ptr<Object> data)
    {
      assert(is_caller(n));
      if (auto call = get_call(n))
        call->callee.set_property(name, std::move(data));
    }

    void remove_callee_property(const node_handle& n, const std::string& name)
    {
      assert(is_caller(n));
      if (auto call = get_call(n))
        call->callee.remove_property(name);
    }

  public:
    auto get_index(const node_handle& node, const socket_handle& socket) const
      -> size_t
    {
      if (ng.is_input_socket(socket)) {
        auto ss = ng.input_sockets(node);
        return rn::distance(ss.begin(), rn::find(ss, socket));
      }
      if (ng.is_output_socket(socket)) {
        auto ss = ng.output_sockets(node);
        return rn::distance(ss.begin(), rn::find(ss, socket));
      }
      unreachable();
    }

    auto get_index(const socket_handle& socket) const
    {
      return get_index(get_node(socket), socket);
    }

  public:
    auto get_path(const node_handle& node) const -> std::optional<std::string>
    {
      assert(is_caller(node));

      node_handle n = node;
      std::vector<std::string> queue;

      while (true) {

        assert(is_caller(n));

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

      auto path = queue           //
                  | rv::reverse   //
                  | rv::join('.') //
                  | rn::to<std::string>;

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
                 | rv::filter([&](auto&& n) { return is_defcall(n); })
                 | rn::to_vector;
        }

        auto pos = sv.find_first_of('.');

        // find name
        if (pos == sv.npos) {
          return g->nodes //
                 | rv::filter([&](auto&& n) {
                     return is_defcall(n) && *ng.get_name(n) == sv;
                   })
                 | rn::to_vector;
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
    auto create_io_bit(const std::string& name)
    {
      auto decl = get_declaration(type_c<node::NodeGroupIOBit>);
      auto bit  = ng.add(decl.node_name(), {name}, {name}, node_type::normal);

      assert(bit);

      set_data(ng.input_sockets(bit)[0], make_object<SocketData>());
      set_data(ng.output_sockets(bit)[0], make_object<SocketData>());

      return bit;
    }

  private:
    /// create new group callee
    auto add_new_callee_group(
      const std::string& name,
      const std::vector<std::string>& iss,
      const std::vector<std::string>& oss) -> node_group*
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
      auto g = check(ng.add(name, {}, {}, node_type::interface));
      auto i = check(ng.add(i_decl.node_name(), {}, {}, node_type::interface));
      auto o = check(ng.add(o_decl.node_name(), {}, {}, node_type::interface));

      auto gdata = make_node_data(node_group {
        .node           = g,
        .dependency     = d,
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

      for (auto&& s : iss) {
        auto bit = create_io_bit(s);
        check(ng.attach_interface(g, ng.input_sockets(bit)[0]));
        check(ng.attach_interface(i, ng.output_sockets(bit)[0]));
        pgdata->input_bits.push_back(bit);
      }

      for (auto&& s : oss) {
        auto bit = create_io_bit(s);
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
    auto add_new_callee_group() -> node_group*
    {
      auto decl = get_declaration(type_c<node::NodeGroupInterface>);
      return add_new_callee_group(
        decl.node_name(), decl.input_sockets(), decl.output_sockets());
    }

    /// create new function callee
    auto add_new_callee_function(
      const std::string& name,
      const std::vector<std::string>& iss,
      const std::vector<std::string>& oss) -> node_function*
    {
      auto d_decl = get_declaration(type_c<node::NodeDependency>);

      auto dep = ng.add(
        d_decl.node_name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal);

      auto body = ng.add(name, iss, oss, node_type::normal);

      assert(body && dep);
      assert(!ng.get_data(body));

      auto bdata = make_node_data(
        node_function {.node = body, .dependency = dep, .callers = {}});

      set_data(body, bdata);

      for (auto&& s : ng.input_sockets(body))
        set_data(s, make_object<SocketData>());

      for (auto&& s : ng.output_sockets(body))
        set_data(s, make_object<SocketData>());

      Info(
        g_logger,
        "Added new function: name={}, id={}",
        *ng.get_name(body),
        to_string(body.id()));

      return &bdata->get<node_function>();
    }

    /// create new macro callee
    auto add_new_callee_macro(
      const std::string& name,
      const std::vector<std::string>& iss,
      const std::vector<std::string>& oss) -> node_macro*
    {
      auto d_decl = get_declaration(type_c<node::NodeDependency>);

      auto dep = ng.add(
        d_decl.node_name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal);

      auto body = ng.add(name, iss, oss, node_type::normal);

      assert(body && dep);
      assert(!ng.get_data(body));

      auto bdata = make_node_data(node_macro {.node = body, .dependency = dep});

      set_data(body, bdata);

      for (auto&& s : ng.input_sockets(body))
        set_data(s, make_object<SocketData>());

      for (auto&& s : ng.output_sockets(body))
        set_data(s, make_object<SocketData>());

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
      callee.visit([&](auto& p) { remove_callee(p); });
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

    /// create new dependency for node call
    /// \param parent parent group to add new call
    /// \param callee callee of new call
    /// \returns dependency node
    auto add_new_call_dependency(node_group* parent, node_callee callee)
      -> node_handle
    {
      assert(parent);

      auto d_decl = get_declaration(type_c<node::NodeDependency>);

      auto dep = check(ng.add(
        d_decl.node_name(),
        d_decl.input_sockets(),
        d_decl.output_sockets(),
        node_type::normal));

      // check dependency (ignore when it's root call)
      auto valid = callee.visit([&](auto* p) {
        assert(p);
        // call -> parent group
        check(ng.connect(
          ng.output_sockets(dep)[0], ng.input_sockets(parent->dependency)[0]));
        // caleee -> call
        return ng.connect(
          ng.output_sockets(p->dependency)[0], ng.input_sockets(dep)[0]);
      });

      // closed loop
      if (!valid) {
        Error(
          g_logger, "Failed to add node call: recursive call is not allowed");
        ng.remove(dep);
        return {};
      }
      return dep;
    }

    /// create new defcall in group.
    /// \param parent paretn group
    /// \param callee callee node function or group
    auto add_new_call(
      node_group* parent,
      node_callee callee,
      uid id = uid::random_generate()) -> node_call*
    {
      assert(parent);

      auto dep = add_new_call_dependency(parent, callee);

      if (!dep)
        return nullptr;

      auto info =
        check(callee.visit([&](auto p) { return ng.get_info(p->node); }));

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
        auto bit = create_io_bit(*ng.get_name(s));
        check(ng.attach_interface(n, ng.input_sockets(bit)[0]));
        ibits.push_back(bit);
      }

      for (auto&& s : info->output_sockets()) {
        auto bit = create_io_bit(*ng.get_name(s));
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

      auto newcall = &std::get<node_call>(*ndata);

      // add to parent
      parent->add_member(n);
      // set caller pointer
      callee.add_caller(newcall);

      assert(newcall->is_defcall());

      auto name_used = [&](const auto& name) {
        auto ns =
          parent->nodes
          | rv::filter([&](auto&& n) { return n != newcall->node; })
          | rv::filter([&](auto&& n) { return ng.get_name(n) == name; });
        return !ns.empty();
      };

      auto make_fresh_name = [&](const auto& name) {
        // escape string
        auto tmp   = name;
        auto count = 1;
        while (name_used(tmp)) {
          tmp = fmt::format("{}{}", name, count);
          ++count;
        }
        return tmp;
      };

      callee.visit([&](auto& p) {
        auto name = make_fresh_name(info->name());
        ng.set_name(n, name);
        ng.set_name(p->node, name);
      });

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
      callee.visit([&](auto* p) {
        if (p->callers.empty())
          remove_callee(p);
      });
    }

    // copy node call
    auto copy_call(
      node_group* parent,
      node_call* call,
      uid id = uid::random_generate()) -> node_call*
    {
      assert(parent);

      auto callee = call->callee;
      auto dep    = add_new_call_dependency(parent, callee);

      if (!dep)
        return nullptr;

      // create interface
      auto info = ng.get_info(call->node);
      auto n    = ng.add(info->name(), {}, {}, node_type::interface, id);

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
        auto bit = create_io_bit(*ng.get_name(s));
        check(ng.attach_interface(n, ng.input_sockets(bit)[0]));
        ibits.push_back(bit);
      }

      for (auto&& s : info->output_sockets()) {
        auto bit = create_io_bit(*ng.get_name(s));
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

      auto newc = &std::get<node_call>(*ndata);

      // add to parent
      parent->add_member(n);
      // set caller pointer
      callee.add_caller(newc);

      assert(call->input_bits.size() == newc->input_bits.size());
      assert(call->output_bits.size() == newc->output_bits.size());

      auto iss = ng.input_sockets(call->node);
      auto oss = ng.output_sockets(call->node);

      assert(iss.size() == newc->input_bits.size());
      assert(oss.size() == newc->output_bits.size());

      // clone node property
      for (auto&& [key, v] : call->properties) {
        set_caller_property(newc->node, key, v.clone());
      }

      // clone socket property
      for (auto&& [idx, s] : iss | rv::enumerate) {
        for (auto&& [key, v] : get_data(s)->properties) {
          set_caller_property(
            ng.input_sockets(newc->node)[idx], key, v.clone());
        }
      }

      // clone socket property
      for (auto&& [idx, s] : oss | rv::enumerate) {
        for (auto&& [key, v] : get_data(s)->properties) {
          set_caller_property(
            ng.output_sockets(newc->node)[idx], key, v.clone());
        }
      }

      return newc;
    }

    // clone node call
    auto clone_call(
      node_group* parent,
      node_call* call,
      uid id = uid::random_generate()) -> node_call*
    {
      return call->callee.visit(
        [&](auto* p) { return clone_call(parent, call, p, id); });
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
      auto newg = add_new_callee_group();

      // copy node names
      ng.set_name(newg->node, *ng.get_name(callee->node));
      ng.set_name(newg->input_handler, *ng.get_name(callee->input_handler));
      ng.set_name(newg->output_handler, *ng.get_name(callee->output_handler));

      // copy/clone members
      for (auto&& n : callee->members) {
        assert(is_caller(n));

        auto newn = clone_call(newg, get_call(n))->node;
        assert(is_caller(newn));

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

      { // clone properties
        for (auto&& [key, v] : call->callee.properties())
          set_callee_property(newc->node, key, v.clone());

        for (auto&& [key, v] : call->properties)
          set_caller_property(newc->node, key, v.clone());

        for (auto&& s : ng.input_sockets(call->node)) {
          auto news =
            check(add_input_socket(newc->node, *ng.get_name(s), size_t(-1)));
          for (auto&& [key, v] : get_data(s)->properties)
            set_caller_property(news, key, v.clone());
        }

        for (auto&& s : ng.output_sockets(call->node)) {
          auto news =
            check(add_output_socket(newc->node, *ng.get_name(s), size_t(-1)));
          for (auto&& [key, v] : get_data(s)->properties)
            set_caller_property(news, key, v.clone());
        }
      }

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
    auto get_node_type(const node_handle& node) const -> structured_node_type
    {
      assert(is_caller(node));
      if (is_function_call(node))
        return structured_node_type::function;
      if (is_macro_call(node))
        return structured_node_type::macro;
      if (is_group_call(node))
        return structured_node_type::group;
      if (is_group_input(node))
        return structured_node_type::group_input;
      if (is_group_output(node))
        return structured_node_type::group_output;
      unreachable();
    }

    auto get_call_type(const node_handle& node) const -> structured_call_type
    {
      auto call_type = structured_call_type::call;

      if (is_defcall(node))
        call_type = structured_call_type::definition;

      return call_type;
    }

    auto get_socket_type(const socket_handle& socket) const
      -> structured_socket_type
    {
      if (ng.is_input_socket(socket))
        return structured_socket_type::input;
      if (ng.is_output_socket(socket))
        return structured_socket_type::output;
      unreachable();
    }

  public:
    auto get_definition(const node_handle& node) const -> node_handle
    {
      assert(is_caller(node));

      if (auto call = get_call(node))
        return call->callee.get_defcall()->node;

      return {};
    }

    auto get_calls(const node_handle& node) const -> std::vector<node_handle>
    {
      assert(is_caller(node));

      if (auto call = get_call(node))
        return call->callee.visit([](auto* p) {
          auto&& cs = p->callers;
          assert(!cs.empty());
          return rn::subrange(cs.begin() + 1, cs.end())
                 | rv::transform([](auto* call) { return call->node; })
                 | rn::to_vector;
        });

      return {};
    }

    auto get_group_members(const node_handle& node) const
      -> std::vector<node_handle>
    {
      assert(is_caller(node));

      if (auto g = get_callee_group(node))
        return g->members;

      return {};
    }

    auto get_group_input(const node_handle& node) const -> node_handle
    {
      assert(is_caller(node));

      if (auto g = get_callee_group(node))
        return g->input_handler;

      return {};
    }

    auto get_group_output(const node_handle& node) const -> node_handle
    {
      assert(is_caller(node));

      if (auto g = get_callee_group(node))
        return g->output_handler;

      return {};
    }

    auto get_group_nodes(const node_handle& node) const
      -> std::vector<node_handle>
    {
      assert(is_caller(node));

      if (auto g = get_callee_group(node))
        return g->nodes;

      return {};
    }

    auto get_parent_group(const node_handle& node) const -> node_handle
    {
      assert(is_caller(node));

      if (auto call = get_call(node))
        if (call->parent != get_group(root))
          return call->parent->get_defcall()->node;

      if (auto io = get_io(node))
        return io->parent->get_defcall()->node;

      return {};
    }

  public:
    void set_name(const node_handle& node, const std::string& name)
    {
      assert(is_caller(node));

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
      auto node = get_node(socket);
      auto idx  = get_index(node, socket);

      assert(is_caller(node));

      static const auto re = std::regex(socket_name_regex);

      if (!std::regex_match(name, re)) {
        Error(g_logger, "Invalid socket name: {}", name);
        return;
      }

      auto set_bit_name = [&](auto&& bit, auto&& name) {
        ng.set_name(ng.input_sockets(bit)[0], name);
        ng.set_name(ng.output_sockets(bit)[0], name);
      };

      // macro
      if (auto m = get_callee_macro(node)) {

        auto caller = get_call(node);
        assert(caller);

        if (ng.is_input_socket(socket)) {
          auto bit = caller->input_bits[idx];
          set_bit_name(bit, name);
        }

        if (ng.is_output_socket(socket)) {
          auto bit = caller->output_bits[idx];
          set_bit_name(bit, name);
        }
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
          set_bit_name(g->input_bits[idx], name);

          for (auto&& caller : g->callers) {
            // set caller socket names
            set_bit_name(caller->input_bits[idx], name);
          }
        }

        if (ng.is_output_socket(socket)) {
          // set group socket name
          set_bit_name(g->output_bits[idx], name);

          for (auto&& caller : g->callers) {
            // set caller socket names
            set_bit_name(caller->output_bits[idx], name);
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
      auto newbit = create_io_bit(name);
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
      auto newbit = create_io_bit(name);
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
      assert(is_caller(node));

      // io (redirect to group)
      if (auto io = get_io(node)) {
        if (io->is_output()) {
          auto outer =
            add_output_socket(io->parent->get_defcall()->node, socket, index);
          return ng.input_sockets(node)[get_index(outer)];
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
      assert(is_caller(node));

      // io (redirect to group)
      if (auto io = get_io(node)) {
        if (io->is_input()) {
          auto outer =
            add_input_socket(io->parent->get_defcall()->node, socket, index);
          return ng.output_sockets(node)[get_index(outer)];
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

      auto node = get_node(socket);
      auto idx  = get_index(node, socket);
      auto type = ng.get_info(socket)->type();

      assert(is_caller(node));

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

          auto newg = add_new_callee_group();
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
    auto create_function_declaration(
      const std::string& node_path,
      const std::string& node_name,
      const std::vector<std::string>& iss,
      const std::vector<std::string>& oss) -> node_handle
    {
      auto full_name = node_path + "." + node_name;

      Info(g_logger, "Creating new function declaration: {}", full_name);

      if (auto p = create_declaration_path(full_name)) {
        auto func = check(add_new_callee_function(node_name, iss, oss));
        auto call = check(add_new_call(p, func));
        return call->node;
      }
      return {};
    }

    auto create_macro_declaration(
      const std::string& node_path,
      const std::string& node_name,
      const std::vector<std::string>& iss,
      const std::vector<std::string>& oss) -> node_handle
    {
      auto full_name = node_path + "." + node_name;

      Info(g_logger, "Creating new macro declaration: {}", full_name);

      if (auto p = create_declaration_path(full_name)) {
        auto func = check(add_new_callee_macro(node_name, iss, oss));
        auto call = check(add_new_call(p, func));
        return call->node;
      }
      return {};
    }

    auto create_group_declaration(
      const std::string& node_path,
      const std::string& node_name,
      const std::vector<std::string>& iss,
      const std::vector<std::string>& oss) -> node_handle
    {
      auto full_name = node_path + "." + node_name;

      Info(g_logger, "Creating new group declaration: {}", full_name);

      if (auto p = create_declaration_path(full_name)) {
        auto func = check(add_new_callee_group(node_name, iss, oss));
        auto call = check(add_new_call(p, func));
        return call->node;
      }
      return {};
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
      auto newg = add_new_callee_group();
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
        if (rn::find(cs, c) == cs.end())
          cs.push_back(c);
      };

      for (auto&& n : nodes) {
        for (auto&& c : ng.input_connections(n)) {
          auto info = ng.get_info(c);
          assert(info->src_interfaces().size() == 1);
          if (rn::find(nodes, info->src_interfaces()[0]) == nodes.end())
            addc(ics, c);
        }
        for (auto&& c : ng.output_connections(n)) {
          auto info = ng.get_info(c);
          assert(info->dst_interfaces().size() == 1);
          if (rn::find(nodes, info->dst_interfaces()[0]) == nodes.end())
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
      assert(is_caller(src));

      if (parent_group)
        assert(is_caller(parent_group));

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
      assert(is_caller(src));

      if (parent_group)
        assert(is_caller(parent_group));

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
      assert(is_caller(node));

      if (auto call = get_call(node))
        return remove_call(call);

      Error(g_logger, "This node cannot be removed");
    }

    auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket,
      const uid& id) -> connection_handle
    {
      auto srcn = get_node(src_socket);
      auto dstn = get_node(dst_socket);

      assert(is_caller(srcn));
      assert(is_caller(dstn));

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
      assert(is_caller(node));

      if (auto call = get_call(node))
        if (call->parent != get_group(root))
          call->parent->bring_front(node);

      if (auto io = get_io(node))
        io->parent->bring_front(node);
    }

    void bring_back(const node_handle& node)
    {
      assert(is_caller(node));

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

      return structured_node_graph(std::make_unique<impl_wrap>(std::move(ret)));
    }

    void clear()
    {
      ng.clear();
      init();
    }
  };

  // wrapper for public graph interface
  class structured_node_graph::impl_wrap
  {
    impl m_impl;

  public:
    impl_wrap() = default;

    impl_wrap(impl&& other) noexcept
      : m_impl {std::move(other)}
    {
    }

  public:
    template <class Handle>
    bool exists(const Handle& h) const
    {
      return m_impl.ng.exists(h);
    }

  public:
    auto node(const uid& id) const -> node_handle
    {
      return m_impl.ng.node(id);
    }

    auto socket(const uid& id) const -> socket_handle
    {
      return m_impl.ng.socket(id);
    }

    auto connection(const uid& id) const -> connection_handle
    {
      return m_impl.ng.connection(id);
    }

  public:
    auto get_info(const node_handle& node) const
      -> std::optional<structured_node_info>
    {
      if (!exists(node))
        return std::nullopt;

      auto info = m_impl.ng.get_info(node);
      assert(info);

      auto ntype = m_impl.get_node_type(node);
      auto ctype = m_impl.get_call_type(node);

      return structured_node_info(
        info->name(),
        info->input_sockets(),
        info->output_sockets(),
        ntype,
        ctype);
    }

    auto get_info(const socket_handle& socket) const
      -> std::optional<structured_socket_info>
    {
      if (!exists(socket))
        return std::nullopt;

      auto n = m_impl.get_node(socket);
      assert(n);

      auto info = m_impl.ng.get_info(socket);
      assert(info);

      auto stype = m_impl.get_socket_type(socket);
      auto cs    = m_impl.ng.connections(socket);

      return structured_socket_info(
        info->name(), stype, n, m_impl.get_index(n, socket), cs);
    }

    auto get_info(const connection_handle& connection) const
      -> std::optional<structured_connection_info>
    {
      if (!exists(connection))
        return std::nullopt;

      auto info = m_impl.ng.get_info(connection);
      assert(info);

      return structured_connection_info(
        m_impl.get_node(info->src_socket()),
        info->src_socket(),
        m_impl.get_node(info->dst_socket()),
        info->dst_socket());
    }

  public:
    template <class Handle>
    auto get_name(const Handle& h) const -> std::optional<std::string>
    {
      return m_impl.ng.get_name(h);
    }

    template <class Handle>
    void set_name(const Handle& h, const std::string& name)
    {
      if (!exists(h))
        return;

      m_impl.set_name(h, name);
    }

    auto get_pos(const node_handle& node) const -> std::optional<glm::dvec2>
    {
      if (!exists(node))
        return std::nullopt;

      if (auto prop = m_impl.get_caller_property(node, "__pos"))
        if (auto vec = value_cast_if<Vec2>(prop))
          return *vec;

      return glm::dvec2();
    }

    void set_pos(const node_handle& node, const glm::dvec2& newpos)
    {
      if (!exists(node))
        return;

      if (auto prop = m_impl.get_caller_property(node, "__pos")) {
        if (auto vec = value_cast_if<Vec2>(prop)) {
          *vec = newpos;
          return;
        }
      }

      // add new prop
      auto vec = make_object<Vec2>(newpos);
      m_impl.set_caller_property(node, "__pos", vec);
    }

    auto get_arg(const socket_handle& socket) const -> object_ptr<Object>
    {
      if (!exists(socket))
        return {};

      return m_impl.get_caller_property(socket, "__arg");
    }

    void set_arg(const socket_handle& socket, object_ptr<Object> data)
    {
      if (!exists(socket))
        return;

      m_impl.set_caller_property(socket, "__arg", std::move(data));
    }

    template <class Handle>
    auto get_source_id(const Handle& h) -> uid
    {
      if (!exists(h))
        return {};

      if (auto prop = m_impl.get_caller_property(h, "__src"))
        return uid {*value_cast<UInt64>(prop)};

      return h.id();
    }

    template <class Handle>
    void set_source_id(const Handle& h, uid id)
    {
      if (!exists(h))
        return;

      if (auto prop = m_impl.get_caller_property(h, "__src")) {
        *value_cast<UInt64>(prop) = id.data;
        return;
      }

      m_impl.set_caller_property(h, "__src", make_object<UInt64>(id.data));
    }

    auto get_node_declaration(const node_handle& n) const
      -> std::shared_ptr<const node_declaration>
    {
      if (!exists(n))
        return nullptr;

      if (auto prop = m_impl.get_callee_property(n, "__decl"))
        return *value_cast<NodeDeclData>(prop);

      return nullptr;
    }

    template <class Handle>
    auto _get_property(const Handle& h, const std::string& name) const
      -> object_ptr<Object>
    {
      if (!exists(h))
        return nullptr;

      return m_impl.get_caller_property(h, name);
    }

    template <class Handle>
    void set_property(
      const Handle& h,
      const std::string& name,
      object_ptr<Object> data)
    {
      if (!exists(h))
        return;

      m_impl.set_caller_property(h, name, std::move(data));
    }

    template <class Handle>
    void remove_property(const Handle& h, const std::string& name)
    {
      m_impl.remove_caller_property(h, name);
    }

    auto _get_shared_property(const node_handle& h, const std::string& name)
      const -> object_ptr<Object>
    {
      if (!exists(h))
        return nullptr;

      return m_impl.get_callee_property(h, name);
    }

    void set_shared_property(
      const node_handle& h,
      const std::string& name,
      object_ptr<Object> data)
    {
      if (!exists(h))
        return;

      m_impl.set_callee_property(h, name, std::move(data));
    }

    void remove_shared_property(const node_handle& h, const std::string& name)
    {
      m_impl.remove_callee_property(h, name);
    }

    auto get_index(const socket_handle& socket) const -> std::optional<size_t>
    {
      if (!exists(socket))
        return std::nullopt;

      return m_impl.get_index(socket);
    }

    auto get_path(const node_handle& node) const -> std::optional<std::string>
    {
      if (!exists(node))
        return std::nullopt;

      return m_impl.get_path(node);
    }

    auto search_path(const std::string& path) const -> std::vector<node_handle>
    {
      return m_impl.search_path(path);
    }

    bool is_parent_of(const node_handle& parent, const node_handle& child) const
    {
      if (!exists(parent) || !exists(child))
        return false;

      return m_impl.is_parent_of(parent, child);
    }

    bool is_child_of(const node_handle& child, const node_handle& parent) const
    {
      if (!exists(child) || !exists(parent))
        return false;

      return m_impl.is_child_of(child, parent);
    }

    auto node(const socket_handle& socket) const -> node_handle
    {
      if (!exists(socket))
        return {};

      return m_impl.get_node(socket);
    }

    auto input_sockets(const node_handle& node) const
      -> std::vector<socket_handle>
    {
      return m_impl.ng.input_sockets(node);
    }

    auto output_sockets(const node_handle& node) const
      -> std::vector<socket_handle>
    {
      return m_impl.ng.output_sockets(node);
    }

    auto input_connections(const node_handle& node) const
      -> std::vector<connection_handle>
    {
      return m_impl.ng.input_connections(node);
    }

    auto output_connections(const node_handle& node) const
      -> std::vector<connection_handle>
    {
      return m_impl.ng.output_connections(node);
    }

    auto connections(const socket_handle& socket) const
      -> std::vector<connection_handle>
    {
      return m_impl.ng.connections(socket);
    }

    bool is_definition(const node_handle& node) const
    {
      if (!exists(node))
        return false;

      return m_impl.is_defcall(node);
    }

    auto get_definition(const node_handle& node) const -> node_handle
    {
      if (!exists(node))
        return {};

      return m_impl.get_definition(node);
    }

    bool is_call(const node_handle& node) const
    {
      if (!exists(node))
        return false;

      return m_impl.is_call(node);
    }

    auto get_calls(const node_handle& node) const -> std::vector<node_handle>
    {
      if (!exists(node))
        return {};

      return m_impl.get_calls(node);
    }

    bool is_function(const node_handle& node) const
    {
      if (!exists(node))
        return false;

      return m_impl.is_function_call(node);
    }

    bool is_macro(const node_handle& node) const
    {
      if (!exists(node))
        return false;

      return m_impl.is_macro_call(node);
    }

    bool is_group(const node_handle& node) const
    {
      if (!exists(node))
        return false;

      return m_impl.is_group_call(node);
    }

    bool is_group_member(const node_handle& node) const
    {
      if (!exists(node))
        return false;

      return m_impl.is_group_member(node);
    }

    bool is_group_output(const node_handle& node) const
    {
      if (!exists(node))
        return false;

      return m_impl.is_group_output(node);
    }

    bool is_group_input(const node_handle& node) const
    {
      if (!exists(node))
        return false;

      return m_impl.is_group_input(node);
    }

    auto get_group_members(const node_handle& node) const
      -> std::vector<node_handle>
    {
      if (!exists(node))
        return {};

      return m_impl.get_group_members(node);
    }

    auto get_group_input(const node_handle& node) const -> node_handle
    {
      if (!exists(node))
        return {};

      return m_impl.get_group_input(node);
    }

    auto get_group_output(const node_handle& node) const -> node_handle
    {
      if (!exists(node))
        return {};

      return m_impl.get_group_output(node);
    }

    auto get_group_nodes(const node_handle& node) const
      -> std::vector<node_handle>
    {
      if (!exists(node))
        return {};

      return m_impl.get_group_nodes(node);
    }

    auto get_parent_group(const node_handle& node) const -> node_handle
    {
      if (!exists(node))
        return {};

      return m_impl.get_parent_group(node);
    }

    auto add_input_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index) -> socket_handle
    {
      if (!exists(group))
        return {};

      return m_impl.add_input_socket(group, socket, index);
    }

    auto add_output_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index) -> socket_handle
    {
      if (!exists(group))
        return {};

      return m_impl.add_output_socket(group, socket, index);
    }

    void remove_socket(const socket_handle& socket)
    {
      if (!exists(socket))
        return;

      return m_impl.remove_socket(socket);
    }

    void bring_front(const node_handle& node)
    {
      if (!exists(node))
        return;

      m_impl.bring_front(node);
    }

    void bring_back(const node_handle& node)
    {
      if (!exists(node))
        return;

      m_impl.bring_back(node);
    }

    auto create_declaration(
      const std::shared_ptr<const node_declaration>& decl,
      structured_node_graph& ng) -> node_handle
    {
      return decl->visit( //
        overloaded        //
        {[&](const composed_node_declaration& d) {
           auto n = m_impl.create_group_declaration(
             d.node_path(),
             d.node_name(),
             d.input_sockets(),
             d.output_sockets());

           if (n) {

             // init composed group
             if (!d.init_composed(ng, n)) {
               destroy(n);
               return node_handle();
             }

             // set default args (init time only)
             for (auto&& [idx, arg] : d.default_args()) {
               set_arg(input_sockets(n)[idx], arg.clone());
             }

             m_impl.set_callee_property(
               n, "__decl", make_object<NodeDeclData>(decl));
           }
           return n;
         },
         [&](const function_node_declaration& d) {
           auto n = m_impl.create_function_declaration(
             d.node_path(),
             d.node_name(),
             d.input_sockets(),
             d.output_sockets());

           if (n) {
             // set default args
             for (auto&& [idx, arg] : d.default_args()) {
               set_arg(input_sockets(n)[idx], arg.clone());
             }

             m_impl.set_callee_property(
               n, "__decl", make_object<NodeDeclData>(decl));
           }
           return n;
         },
         [&](const macro_node_declaration& d) {
           auto n = m_impl.create_macro_declaration(
             d.node_path(),
             d.node_name(),
             d.input_sockets(),
             d.output_sockets());

           if (n) {
             m_impl.set_callee_property(
               n, "__decl", make_object<NodeDeclData>(decl));
           }
           return n;
         }});
    }

    auto create_group(
      const node_handle& parent_group,
      const std::vector<node_handle>& nodes,
      const uid& id) -> node_handle
    {
      if (parent_group && !exists(parent_group))
        return {};

      for (auto&& n : nodes)
        if (!exists(n))
          return {};

      return m_impl.create_group(parent_group, nodes, id);
    }

    auto create_copy(
      const node_handle& parent_group,
      const node_handle& src,
      const uid& id) -> node_handle
    {
      if (parent_group && !exists(parent_group))
        return {};

      if (!exists(src))
        return {};

      return m_impl.create_copy(parent_group, src, id);
    }

    auto create_clone(
      const node_handle& parent_group,
      const node_handle& src,
      const uid& id) -> node_handle
    {
      if (parent_group && !exists(parent_group))
        return {};

      if (!exists(src))
        return {};

      return m_impl.create_clone(parent_group, src, id);
    }

    void destroy(const node_handle& node)
    {
      if (!exists(node))
        return;

      m_impl.destroy(node);
    }

    auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket,
      const uid& id) -> connection_handle
    {
      if (!exists(src_socket) || !exists(dst_socket))
        return {};

      return m_impl.connect(src_socket, dst_socket, id);
    }

    void disconnect(const connection_handle& c)
    {
      if (!exists(c))
        return;

      return m_impl.disconnect(c);
    }

    auto clone() const -> structured_node_graph
    {
      return m_impl.clone();
    }

    void clear()
    {
      m_impl.clear();
    }
  };

  structured_node_graph::structured_node_graph()
    : m_pimpl {std::make_unique<impl_wrap>()}
  {
  }

  structured_node_graph::structured_node_graph(
    std::unique_ptr<impl_wrap>&& pimpl)
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
    return m_pimpl->exists(node);
  }

  bool structured_node_graph::exists(const connection_handle& connection) const
  {
    return m_pimpl->exists(connection);
  }

  bool structured_node_graph::exists(const socket_handle& socket) const
  {
    return m_pimpl->exists(socket);
  }

  auto structured_node_graph::node(const uid& id) const -> node_handle
  {
    return m_pimpl->node(id);
  }

  auto structured_node_graph::socket(const uid& id) const -> socket_handle
  {
    return m_pimpl->socket(id);
  }

  auto structured_node_graph::connection(const uid& id) const
    -> connection_handle
  {
    return m_pimpl->connection(id);
  }

  auto structured_node_graph::get_info(const node_handle& node) const
    -> std::optional<structured_node_info>
  {
    return m_pimpl->get_info(node);
  }

  auto structured_node_graph::get_info(const socket_handle& socket) const
    -> std::optional<structured_socket_info>
  {
    return m_pimpl->get_info(socket);
  }

  auto structured_node_graph::get_info(const connection_handle& connection)
    const -> std::optional<structured_connection_info>
  {
    return m_pimpl->get_info(connection);
  }

  auto structured_node_graph::get_name(const node_handle& node) const
    -> std::optional<std::string>
  {
    return m_pimpl->get_name(node);
  }

  auto structured_node_graph::get_name(const socket_handle& socket) const
    -> std::optional<std::string>
  {
    return m_pimpl->get_name(socket);
  }

  void structured_node_graph::set_name(
    const node_handle& node,
    const std::string& name)
  {
    m_pimpl->set_name(node, name);
  }

  void structured_node_graph::set_name(
    const socket_handle& socket,
    const std::string& name)
  {
    m_pimpl->set_name(socket, name);
  }

  auto structured_node_graph::_get_property(
    const node_handle& h,
    const std::string& name) const -> object_ptr<Object>
  {
    return m_pimpl->_get_property(h, name);
  }

  auto structured_node_graph::_get_property(
    const socket_handle& h,
    const std::string& name) const -> object_ptr<Object>
  {
    return m_pimpl->_get_property(h, name);
  }

  void structured_node_graph::set_property(
    const node_handle& h,
    const std::string& name,
    object_ptr<Object> data)
  {
    m_pimpl->set_property(h, name, std::move(data));
  }

  void structured_node_graph::set_property(
    const socket_handle& h,
    const std::string& name,
    object_ptr<Object> data)
  {
    m_pimpl->set_property(h, name, std::move(data));
  }

  void structured_node_graph::remove_property(
    const node_handle& h,
    const std::string& name)
  {
    m_pimpl->remove_property(h, name);
  }

  void structured_node_graph::remove_property(
    const socket_handle& h,
    const std::string& name)
  {
    m_pimpl->remove_property(h, name);
  }

  auto structured_node_graph::_get_shared_property(
    const node_handle& h,
    const std::string& name) const -> object_ptr<Object>
  {
    return m_pimpl->_get_shared_property(h, name);
  }

  void structured_node_graph::set_shared_property(
    const node_handle& h,
    const std::string& name,
    object_ptr<Object> data)
  {
    m_pimpl->set_shared_property(h, name, std::move(data));
  }

  void structured_node_graph::remove_shared_property(
    const node_handle& h,
    const std::string& name)
  {
    m_pimpl->remove_shared_property(h, name);
  }

  auto structured_node_graph::get_pos(const node_handle& node) const
    -> std::optional<glm::dvec2>
  {
    return m_pimpl->get_pos(node);
  }

  void structured_node_graph::set_pos(
    const node_handle& node,
    const glm::dvec2& newpos)
  {
    return m_pimpl->set_pos(node, newpos);
  }

  auto structured_node_graph::get_arg(const socket_handle& socket) const
    -> object_ptr<Object>
  {
    return m_pimpl->get_arg(socket);
  }

  void structured_node_graph::set_arg(
    const socket_handle& socket,
    object_ptr<Object> data)
  {
    m_pimpl->set_arg(socket, std::move(data));
  }

  auto structured_node_graph::get_source_id(const node_handle& h) const -> uid
  {
    return m_pimpl->get_source_id(h);
  }

  auto structured_node_graph::get_source_id(const socket_handle& h) const -> uid
  {
    return m_pimpl->get_source_id(h);
  }

  void structured_node_graph::set_source_id(const node_handle& h, uid id)
  {
    m_pimpl->set_source_id(h, id);
  }

  void structured_node_graph::set_source_id(const socket_handle& h, uid id)
  {
    m_pimpl->set_source_id(h, id);
  }

  auto structured_node_graph::get_node_declaration(const node_handle& n) const
    -> std::shared_ptr<const node_declaration>
  {
    return m_pimpl->get_node_declaration(n);
  }

  auto structured_node_graph::get_index(const socket_handle& socket) const
    -> std::optional<size_t>
  {
    return m_pimpl->get_index(socket);
  }

  auto structured_node_graph::get_path(const node_handle& node) const
    -> std::optional<std::string>
  {
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
    return m_pimpl->is_parent_of(parent, child);
  }

  bool structured_node_graph::is_child_of(
    const node_handle& child,
    const node_handle& parent) const
  {
    return m_pimpl->is_child_of(child, parent);
  }

  auto structured_node_graph::node(const socket_handle& socket) const
    -> node_handle
  {
    return m_pimpl->node(socket);
  }

  auto structured_node_graph::input_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return m_pimpl->input_sockets(node);
  }

  auto structured_node_graph::output_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return m_pimpl->output_sockets(node);
  }

  auto structured_node_graph::input_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_pimpl->input_connections(node);
  }

  auto structured_node_graph::output_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    return m_pimpl->output_connections(node);
  }

  auto structured_node_graph::connections(const socket_handle& socket) const
    -> std::vector<connection_handle>
  {
    return m_pimpl->connections(socket);
  }

  bool structured_node_graph::is_definition(const node_handle& node) const
  {
    return m_pimpl->is_definition(node);
  }

  auto structured_node_graph::get_definition(const node_handle& node) const
    -> node_handle
  {
    return m_pimpl->get_definition(node);
  }

  bool structured_node_graph::is_call(const node_handle& node) const
  {
    return m_pimpl->is_call(node);
  }

  auto structured_node_graph::get_calls(const node_handle& node) const
    -> std::vector<node_handle>
  {
    return m_pimpl->get_calls(node);
  }

  bool structured_node_graph::is_function(const node_handle& node) const
  {
    return m_pimpl->is_function(node);
  }

  bool structured_node_graph::is_macro(const node_handle& node) const
  {
    return m_pimpl->is_macro(node);
  }

  bool structured_node_graph::is_group(const node_handle& node) const
  {
    return m_pimpl->is_group(node);
  }

  bool structured_node_graph::is_group_member(const node_handle& node) const
  {
    if (!exists(node))
      return false;

    return m_pimpl->is_group_member(node);
  }

  bool structured_node_graph::is_group_output(const node_handle& node) const
  {
    return m_pimpl->is_group_output(node);
  }

  bool structured_node_graph::is_group_input(const node_handle& node) const
  {
    return m_pimpl->is_group_input(node);
  }

  auto structured_node_graph::get_group_members(const node_handle& node) const
    -> std::vector<node_handle>
  {
    return m_pimpl->get_group_members(node);
  }

  auto structured_node_graph::get_group_input(const node_handle& node) const
    -> node_handle
  {
    return m_pimpl->get_group_input(node);
  }

  auto structured_node_graph::get_group_output(const node_handle& node) const
    -> node_handle
  {
    return m_pimpl->get_group_output(node);
  }

  auto structured_node_graph::get_group_nodes(const node_handle& node) const
    -> std::vector<node_handle>
  {
    return m_pimpl->get_group_nodes(node);
  }

  auto structured_node_graph::get_parent_group(const node_handle& node) const
    -> node_handle
  {
    return m_pimpl->get_parent_group(node);
  }

  auto structured_node_graph::add_input_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index) -> socket_handle
  {
    return m_pimpl->add_input_socket(group, socket, index);
  }

  auto structured_node_graph::add_output_socket(
    const node_handle& group,
    const std::string& socket,
    size_t index) -> socket_handle
  {
    return m_pimpl->add_output_socket(group, socket, index);
  }

  void structured_node_graph::remove_socket(const socket_handle& socket)
  {
    return m_pimpl->remove_socket(socket);
  }

  void structured_node_graph::bring_front(const node_handle& node)
  {
    m_pimpl->bring_front(node);
  }

  void structured_node_graph::bring_back(const node_handle& node)
  {
    m_pimpl->bring_back(node);
  }

  auto structured_node_graph::create_declaration(
    const std::shared_ptr<const node_declaration>& decl) -> node_handle
  {
    return m_pimpl->create_declaration(decl, *this);
  }

  auto structured_node_graph::create_group(
    const node_handle& parent_group,
    const std::vector<node_handle>& nodes,
    const uid& id) -> node_handle
  {
    return m_pimpl->create_group(parent_group, nodes, id);
  }

  auto structured_node_graph::create_copy(
    const node_handle& parent_group,
    const node_handle& src,
    const uid& id) -> node_handle
  {
    return m_pimpl->create_copy(parent_group, src, id);
  }

  auto structured_node_graph::create_clone(
    const node_handle& parent_group,
    const node_handle& src,
    const uid& id) -> node_handle
  {
    return m_pimpl->create_clone(parent_group, src, id);
  }

  void structured_node_graph::destroy(const node_handle& node)
  {
    m_pimpl->destroy(node);
  }

  auto structured_node_graph::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket,
    const uid& id) -> connection_handle
  {
    return m_pimpl->connect(src_socket, dst_socket, id);
  }

  void structured_node_graph::disconnect(const connection_handle& c)
  {
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
YAVE_DECL_TYPE(yave::NodeDeclData, "0bb7f720-fd1b-4e49-bf10-85c6659dafb3");