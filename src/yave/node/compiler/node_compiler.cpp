//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/support/log.hpp>
#include <yave/node/core/socket_instance_manager.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/rts/generalize.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/rts/value_cast.hpp>

#include <range/v3/algorithm.hpp>

namespace {

  // logger
  std::shared_ptr<spdlog::logger> g_logger;

  // init
  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("node_compiler");
      return 1;
    }();
  }
} // namespace

namespace yave {

  auto node_compiler::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock(m_mtx);
  }

  node_compiler::node_compiler()
  {
    init_logger();
  }

  auto node_compiler::get_errors() const -> error_list
  {
    auto lck = _lock();
    return m_errors.clone();
  }

  auto node_compiler::compile(
    parsed_node_graph&& parsed_graph,
    const node_declaration_store& decls,
    const node_definition_store& defs) -> std::optional<executable>
  {
    auto lck = _lock();

    m_errors.clear();

    // early optimize stage?
    auto ng = _optimize_early(std::move(parsed_graph));
    if (!ng) {
      Error(g_logger, "Failed to optmize parsed node graph");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    // type check and create socket instance map
    auto sim = _type(*ng, decls, defs);
    if (!sim) {
      Error(g_logger, "Failed to type node graph");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    // generate executable apply graph
    auto exe = _generate(*ng, *sim);
    if (!exe) {
      Error(g_logger, "Failed to generate apply graph");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    // optimize executable
    exe = _optimize(std::move(*exe), *ng);

    // verbose type check
    auto succ = _verbose_check(*ng, *sim, *exe);
    if (!succ) {
      Error(g_logger, "Verbose check failed");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    return exe;
  }

  // AST optimization stage.
  auto node_compiler::_optimize_early(parsed_node_graph&& parsed_graph)
    -> std::optional<parsed_node_graph>
  {
    // TODO...
    return std::move(parsed_graph);
  }

  // Graph optimization stage.
  auto node_compiler::_optimize(
    executable&& exe,
    const parsed_node_graph& graph) -> executable
  {
    // TODO...
    (void)graph;
    return std::move(exe);
  }

  /*
    Type check prime trees and generate apply graph.
    Assumes prime tree is already successfully parsed by node_parser.
    We use socket_instance_manager to collect all node instances attached to
    specific node/socket combination to share instance objects across multiple
    inputs.

    Type checking each node is done by following algorithm:
    1. Type check all input sockets.
    2. Enumerate avalable definitions.
    4. Type check to type class, and collect valid definitions.
    5. Decide the single most specialized definition from valid definitions.
    6. Store result to cache.
   */
  auto node_compiler::_type(
    const parsed_node_graph& parsed_graph,
    const node_declaration_store& decls,
    const node_definition_store& defs) -> std::optional<socket_instance_manager>
  {
    struct
    {
      // Recursive implementation of node type checker.
      object_ptr<const Type> rec(
        const node_handle& node,             /* target node */
        const socket_handle& socket,         /* target socket */
        const node_graph& graph,             /* graph */
        const node_declaration_store& decls, /* declarations */
        const node_definition_store& defs,   /* definitions */
        socket_instance_manager& sim,        /* instance cache table (ref) */
        error_list& errors)                  /* error list (ref) */
      {
        auto node_info   = graph.get_info(node);
        auto socket_info = graph.get_info(socket);
        auto node_decl   = decls.find(node_info->name());
        auto node_defs = defs.get_binds(node_info->name(), socket_info->name());
        auto node_os   = graph.output_sockets(node);
        auto node_is   = graph.input_sockets(node);

        // when already in cache
        if (auto inst = sim.find(socket)) {

          Info(
            g_logger,
            "Found instance cache at {}({})#{}",
            node_info->name(),
            to_string(node.id()),
            socket_info->name());

          return inst->type;
        }

        assert(ranges::find(node_os, socket) != node_os.end());
        assert(node_decl);

        // list input info
        auto input_sockets     = std::vector<socket_handle> {};
        auto input_names       = std::vector<std::string> {};
        auto input_connections = std::vector<connection_info> {};
        auto input_types       = std::vector<object_ptr<const Type>> {};

        input_sockets.reserve(node_is.size());
        input_names.reserve(node_is.size());
        input_sockets.reserve(node_is.size());
        input_types.reserve(node_is.size());

        for (auto&& is : node_is) {
          assert(graph.has_connection(is));
          assert(graph.connections(is).size() == 1);
          // socket
          input_sockets.push_back(is);
          // name
          input_names.push_back(*graph.get_name(is));
          // connection
          auto ic = *graph.get_info(graph.connections(is)[0]);
          input_connections.push_back(ic);
          // type (rec call)
          input_types.push_back(rec(
            ic.src_node(), ic.src_socket(), graph, decls, defs, sim, errors));
        }

        // log
        {
          std::string inputs_str;
          for (auto&& i : input_names)
            inputs_str += fmt::format("{} ", i);
          if (input_sockets.empty())
            inputs_str = "(no input)";

          Info(g_logger, "[ Type checking node: id={} ]", to_string(node.id()));
          Info(g_logger, "- name: {}", node_info->name());
          Info(g_logger, "- connected input sockets: {}", inputs_str);
          Info(g_logger, "- output socket: {}", socket_info->name());
          Info(g_logger, "- registered overloadings: {}", node_defs.size());
        }

        auto overloadings =
          std::vector<std::shared_ptr<const node_definition>> {};

        for (auto&& def : node_defs)
          overloadings.push_back(def);

        if (overloadings.empty()) {

          errors.push_back(make_error<compile_error::no_valid_overloading>(
            node, socket, overloadings));

          Error(
            g_logger,
            "[ Typing node {}({})#{} failed ]",
            node_info->name(),
            to_string(node.id()),
            socket_info->name());
          Error(g_logger, "- (No overloading for current input connections)");

          return object_type<Undefined>();
        }

        Info(
          g_logger,
          "[ Overloadings at {}({})#{} ]",
          node_info->name(),
          to_string(node.id()),
          socket_info->name());

        // list overloading instances and its types
        auto overloading_instances = std::vector<object_ptr<const Object>> {};
        auto overloading_types     = std::vector<object_ptr<const Type>> {};

        for (auto&& o : overloadings) {
          auto prim     = graph.get_primitive_container(node);
          auto instance = o->get_instance(prim);
          overloading_instances.push_back(instance);
          overloading_types.push_back(type_of(instance));
        }

        // generalized overloading type.
        // uses type class of declaration here.
        object_ptr<const Type> type_class;
        {
          size_t idx = ranges::distance(
            node_decl->output_sockets().begin(),
            ranges::find(node_decl->output_sockets(), socket_info->name()));

          assert(idx != node_decl->output_sockets().size());
          type_class = node_decl->type_classes()[idx];
        }

        // check overloading types
        for (auto&& type : overloading_types) {
          if (!specializable(type_class, type)) {
            Error(
              g_logger,
              "Invalid node definition: Unification from type class failed, "
              "ignored.");
            Error(g_logger, "- type class: {}", to_string(type_class));
            Error(g_logger, "- definition: {}", to_string(type));
          }
        }

        // log
        {
          std::string input_types_str;
          for (auto&& it : input_types) {
            input_types_str += fmt::format("{}, ", to_string(it));
          }
          if (input_types.empty())
            input_types_str = "(no input type)";

          Info(
            g_logger,
            "[ Type check at node: {}({})#{} ]",
            node_info->name(),
            to_string(node.id()),
            socket_info->name());
          Info(g_logger, "- input types: {}", input_types_str);
          Info(g_logger, "- type class: {}", to_string(type_class));
        }

        // type of target node tree.
        auto node_tp = type_class;
        // updated on each ufinication step
        auto tmp_tp = type_class;

        // Infer type of node on generalized type.
        for (size_t i = 0; i < input_sockets.size(); ++i) {

          auto& it = input_types[i];

          // Solve constrant for each input type.
          try {

            auto v = genvar();
            // [tmp = it -> v]
            auto c = type_constr {tmp_tp, new Type(arrow_type {it, v})};
            // solve constraint
            auto subst = unify({c}, nullptr);
            // update result by substitution.
            node_tp = subst_type_all(subst, node_tp);
            // v contains curried return type.
            tmp_tp = subst_type_all(subst, v);

          } catch (type_error::type_missmatch& e) {

            errors.push_back(make_error<compile_error::type_missmatch>(
              node, input_sockets[i], e.expected(), e.provided()));

            {
              Error(
                g_logger,
                "[ Type check on prime tree failed at {}({})#{} ]",
                node_info->name(),
                to_string(node.id()),
                socket_info->name());
              Error(g_logger, "- on socket: {}", input_names[i]);
              Error(g_logger, "- error type: type_missmatch");
              Error(g_logger, "- socket type: {}", to_string(input_types[i]));
              Error(g_logger, "- expected: {}", to_string(e.expected()));
              Error(g_logger, "- provided: {}", to_string(e.provided()));
            }

            auto flat = flatten(type_class);
            return flat.back();

          } catch (type_error::type_error&) {

            errors.push_back(make_error<compile_error::no_valid_overloading>(
              node, socket, overloadings));

            {
              Error(
                g_logger,
                "[ Type check on prime tree failed at {}({})#{} ]",
                node_info->name(),
                to_string(node.id()),
                socket_info->name());
              Error(g_logger, "- on socket: {}", input_names[i]);
              Error(g_logger, "- error type: unification failed");
              Error(g_logger, "- socket type: {}", to_string(input_types[i]));
            }

            auto flat = flatten(type_class);
            return flat.back();
          }
        }

        {
          Info(
            g_logger,
            "[ Infered type of node: {}({})#{} ]",
            node_info->name(),
            to_string(node.id()),
            socket_info->name());
          Info(g_logger, "- node_tp: {}", to_string(node_tp));
          Info(g_logger, "- tmp_tp: {}", to_string(tmp_tp));
        }

        struct hit_info
        {
          std::shared_ptr<const node_definition> definition;
          object_ptr<const Object> instance;
          object_ptr<const Type> instance_tp;
          object_ptr<const Type> result_tp;
        };

        std::vector<hit_info> hits;
        for (size_t i = 0; i < overloadings.size(); ++i) {
          auto& b = overloadings[i];
          auto& o = overloading_instances[i];
          auto& t = overloading_types[i];

          try {
            auto c         = unify({type_constr {node_tp, t}}, nullptr);
            auto result_tp = subst_type_all(c, tmp_tp);
            hits.push_back({b, o, t, result_tp});
          } catch (type_error::type_error&) {
            continue;
          }
        }

        if (hits.empty()) {

          errors.push_back(make_error<compile_error::no_valid_overloading>(
            node, socket, overloadings));

          {
            Info(
              g_logger,
              "[ Type check on prime tree failed at {}({})#{} ]",
              node_info->name(),
              to_string(node.id()),
              socket_info->name());
            Info(g_logger, "- (No valid overloading found.)");
          }

          return object_type<Undefined>();
        }

        auto ret = hits.front();
        for (size_t i = 1; i < hits.size(); ++i) {
          if (specializable(ret.instance_tp, hits[i].instance_tp)) {
            if (specializable(hits[i].instance_tp, ret.instance_tp)) {

              // TODO: better error info
              errors.push_back(make_error<compile_error::ambiguous_overloading>(
                node, socket, overloadings));

              {
                Error(
                  g_logger,
                  "[ Overloading resolution failed at {}({})#{} ]",
                  node_info->name(),
                  to_string(node.id()),
                  socket_info->name());
                Error(g_logger, "- (Ambiguous overloadings)");
              }

              return object_type<Undefined>();
            }
            ret = hits[i];
          }
        }

        {
          Info(
            g_logger,
            "[ Type of node at {}({})#{} in prime tree successfully deduced ]",
            node_info->name(),
            to_string(node.id()),
            socket_info->name());
          Info(g_logger, "- {}", to_string(ret.result_tp));
        }

        // add instance cache
        sim.add(
          socket, socket_instance {ret.instance, ret.result_tp, ret.definition});

        return ret.result_tp;
      }
    } impl;

    auto& graph      = parsed_graph.graph;
    auto& root_node  = parsed_graph.root;
    auto root_info   = *graph.get_info(root_node);
    auto root_socket = graph.output_sockets(root_node)[0];
    auto sim         = socket_instance_manager();

    auto root_instance =
      impl.rec(root_node, root_socket, graph, decls, defs, sim, m_errors);

    // success
    if (m_errors.empty()) {
      return {std::move(sim)};
    }
    // fail
    return std::nullopt;
  }

  auto node_compiler::_generate(
    const parsed_node_graph& parsed_graph,
    const socket_instance_manager& sim) -> std::optional<executable>
  {
    struct
    {
      auto rec(
        const node_handle& node,
        const socket_handle& socket,
        const node_graph& graph,
        const socket_instance_manager& sim) -> object_ptr<const Object>
      {
        std::vector<object_ptr<const Object>> inputs;

        // recursively build apply graph
        for (auto&& ic : graph.input_connections(node)) {
          auto cinfo = graph.get_info(ic);
          assert(cinfo);
          inputs.push_back(
            rec(cinfo->src_node(), cinfo->src_socket(), graph, sim));
        }

        auto inst = sim.find(socket);

        if (!inst)
          return nullptr;

        object_ptr<const Object> app = inst->instance;

        for (auto&& in : inputs) {
          if (in) {
            // apply
            app = app << in;
          }
        }

        if (!inputs.empty() && app == inst->instance)
          assert(false);

        return app;
      }
    } impl;

    auto& graph = parsed_graph.graph;
    auto& root  = parsed_graph.root;

    auto root_info = graph.get_info(root);

    assert(root_info);
    assert(root_info->output_sockets().size() == 1);

    auto app       = impl.rec(root, graph.output_sockets(root)[0], graph, sim);
    auto root_inst = sim.find(graph.output_sockets(root)[0]);

    assert(root_inst);

    return {{std::move(app), root_inst->type}};
  }

  auto node_compiler::_verbose_check(
    const parsed_node_graph& parsed_graph,
    const socket_instance_manager& sim,
    const executable& exe) -> bool
  {
    try {
      auto tp = type_of(exe.object());
      Info(g_logger, "[ Verbose check result ]");
      Info(g_logger, "- type of apply graph: {}", to_string(tp));

      auto& graph = parsed_graph.graph;
      auto& root  = parsed_graph.root;

      auto inst = sim.find(graph.output_sockets(root)[0]);

      Info(g_logger, "- type of root node:   {}", to_string(inst->type));

      if (same_type(tp, inst->type)) {
        return true;
      }

      Error(g_logger, "[ Verbose check failed ]");
      Error(g_logger, "- Apply graph has different type to the type of root");
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        "Generated apply graph is invalid"));

      return false;

    } catch (...) {

      Error(g_logger, "[ Verbose check failed ]");
      Error(g_logger, "- Exception thrown while checking result");
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        "Exception thrown while checking result"));

      return false;
    }
  }

} // namespace yave