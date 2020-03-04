//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/support/log.hpp>
#include <yave/node/core/socket_instance_manager.hpp>
#include <yave/node/core/node_definition.hpp>
#include <yave/node/core/node_declaration.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/rts/value_cast.hpp>
#include <yave/rts/unit.hpp>
#include <yave/module/std/decl/prim/primitive.hpp>

#include <range/v3/algorithm.hpp>
#include <boost/uuid/uuid_hash.hpp>

YAVE_DECL_G_LOGGER(node_compiler)

using namespace std::string_literals;

namespace yave {

  node_compiler::node_compiler()
  {
    init_logger();
  }

  auto node_compiler::get_errors() const -> error_list
  {
    return m_errors.clone();
  }

  auto node_compiler::compile(
    managed_node_graph&& parsed_graph,
    const node_definition_store& defs) -> std::optional<executable>
  {
    // clear previous errors
    m_errors.clear();

    Info(g_logger, "Start compiling node tree:");
    Info(g_logger, "  Total {} nodes in graph", parsed_graph.nodes().size());
    Info(g_logger, "  Total {} node definitions", defs.size());

    // early optimize stage?
    auto ng = _optimize_early(std::move(parsed_graph));
    if (!ng) {
      Error(g_logger, "Failed to optmize parsed node graph");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    // type check and create apply tree
    auto exe = _type(*ng, defs);
    if (!exe) {
      Error(g_logger, "Failed to type node graph");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    // optimize executable
    exe = _optimize(std::move(*exe), *ng);

    // verbose type check
    auto succ = _verbose_check(*exe, *ng);
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
  auto node_compiler::_optimize_early(managed_node_graph&& parsed_graph)
    -> std::optional<managed_node_graph>
  {
    // TODO...
    return std::move(parsed_graph);
  }

  // Graph optimization stage.
  auto node_compiler::_optimize(
    executable&& exe,
    const managed_node_graph& graph) -> executable
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

    This function is depending on overloading resolution extension in RTS
    module. After typing over gneralized types on each overloaded functions,
    type checker checks existance of appropriate function to call, then replace
    all occurence of overloaded call with the selected closure object. If type
    checking or overloading selection failed, it throws type_error so we need to
    trap them to report to frontend.

    Most of errors thrown from type checker does not relate to node handle, so
    we need to keep track relation between backend object and nodes.
   */

  /// map from Object pointer to source node and socket
  using handle_map_t =
    std::map<object_ptr<const Object>, std::pair<node_handle, socket_handle>>;

  /// map object to (node,socket), otherwise fall back to root.
  auto map_obj_to_node(
    const object_ptr<const Object>& obj,
    const handle_map_t& hmap,
    const managed_node_graph& graph) -> std::pair<node_handle, socket_handle>
  {
    auto it = hmap.find(obj);
    if (it == hmap.end()) {
      auto root = graph.root_group();
      return {root, graph.output_sockets(root)[0]};
    }
    return it->second;
  }

  auto node_compiler::_type(
    const managed_node_graph& parsed_graph,
    const node_definition_store& defs) -> std::optional<executable>
  {
    struct
    {
      // build apply graph
      inline auto rec(
        class_env& env,
        socket_instance_manager& sim,
        handle_map_t hmap,
        const node_definition_store& nds,
        const managed_node_graph& graph,
        const socket_handle& socket) -> object_ptr<const Object>
      {
        assert(graph.get_info(socket)->type() == socket_type::output);

        auto node = graph.node(socket);

        size_t socket_index = 0;
        for (auto&& s : graph.output_sockets(node)) {
          if (s == socket)
            break;
          ++socket_index;
        }

        if (auto inst = sim.find(socket)) {
          return inst->instance;
        }

        auto iss = graph.input_sockets(node);

        // group: make lambda
        if (graph.is_group(node)) {

          // lambda form?
          bool isLambda = false;
          for (auto&& s : iss) {
            if (
              graph.connections(s).empty()
              && value_cast_if<Variable>(graph.get_data(s))) {
              isLambda = true;
              break;
            }
          }

          auto inside = graph.get_group_socket_inside(socket);

          auto body = rec(
            env,
            sim,
            hmap,
            nds,
            graph,
            graph.get_info(graph.connections(inside)[0])->src_socket());

          // apply lambda
          if (isLambda) {
            for (auto iter = iss.rbegin(); iter != iss.rend(); ++iter) {
              auto& s = *iter;
              body    = make_object<Lambda>(
                value_cast<Variable>(graph.get_data(s)), body);
            }
          }

          return body;
        }

        // group input
        if (graph.is_group_input(node)) {

          auto outside = graph.get_group_socket_outside(socket);

          // not connected: return variable
          if (graph.connections(outside).empty())
            return value_cast<Variable>(graph.get_data(outside));

          return rec(
            env,
            sim,
            hmap,
            nds,
            graph,
            graph.get_info(graph.connections(outside)[0])->src_socket());
        }

        // acquire node object

        auto defs = nds.get_binds(*graph.get_name(node), socket_index);

        if (defs.empty())
          throw compile_error::no_valid_overloading(node, socket);

        std::vector<object_ptr<const Object>> insts;
        insts.reserve(defs.size());
        for (auto&& def : defs) {
          insts.push_back(def->instance());
        }

        auto overloaded =
          insts.size() == 1 ? insts[0] : env.add_overloading(insts);

        hmap.emplace(overloaded, std::pair {node, socket});

        // apply inputs
        for (auto&& s : iss) {
          if (graph.connections(s).empty()) {

            // lambda form
            if (!graph.get_data(s))
              return overloaded;

            // apply default argument
            if (auto holder = value_cast_if<DataTypeHolder>(graph.get_data(s)))
              overloaded = overloaded << holder->get_data_constructor();
            else
              overloaded = overloaded << graph.get_data(s);

          } else {
            // normal argument
            assert(graph.connections(s).size() == 1);
            overloaded =
              overloaded << rec(
                env,
                sim,
                hmap,
                nds,
                graph,
                graph.get_info(graph.connections(s)[0])->src_socket());
          }
          hmap.emplace(overloaded, std::pair {node, socket});
        }

        return overloaded;
      }
    } impl;


    auto root_group = parsed_graph.root_group();

    assert(parsed_graph.output_sockets(root_group).size() == 1);
    assert(parsed_graph.input_sockets(root_group).empty());

    auto root_socket = parsed_graph.output_sockets(root_group)[0];

    class_env env;
    socket_instance_manager sim;
    handle_map_t hmap;

    try {

      // build apply tree
      auto app = impl.rec(env, sim, hmap, defs, parsed_graph, root_socket);

      // now we can check type and resolve overloadings
      auto [ty, app2] = type_of_overloaded(app, env);

      // return result tree
      if (m_errors.empty()) {
        Info(
          g_logger, "Successfully type checked node tree! : {}", to_string(ty));
        return executable(app2, ty);
      } else
        return std::nullopt;

      // common type errors
    } catch (const type_error::no_valid_overloading& e) {
      auto [n, s] = map_obj_to_node(e.source(), hmap, parsed_graph);
      m_errors.push_back(make_error<compile_error::no_valid_overloading>(n, s));
    } catch (const type_error::type_missmatch& e) {
      auto [n, s] = map_obj_to_node(e.source(), hmap, parsed_graph);
      m_errors.push_back(make_error<compile_error::type_missmatch>(
        n, s, e.expected(), e.provided()));

      // internal type errors
    } catch (const type_error::unbounded_variable& e) {
      auto [n, s] = map_obj_to_node(e.source(), hmap, parsed_graph);
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        n, s, "Internal type error: unbounded variable detected"));
    } catch (const type_error::circular_constraint& e) {
      auto [n, s] = map_obj_to_node(e.source(), hmap, parsed_graph);
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        n, s, "Internal type error: circular constraint detected"));
    } catch (const type_error::type_error& e) {
      auto [n, s] = map_obj_to_node(e.source(), hmap, parsed_graph);
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        n, s, "Internal type error: "s + e.what()));

      // forward
    } catch (const error_info_base& e) {
      m_errors.push_back(error(e.clone()));

      // others
    } catch (const std::exception& e) {
      auto [n, s] = map_obj_to_node(nullptr, hmap, parsed_graph);
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        n, s, "Internal error: Unknown std::exception detected: "s + e.what()));
    } catch (...) {
      auto [n, s] = map_obj_to_node(nullptr, hmap, parsed_graph);
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        n, s, "Internal error: Unknown exception detected"));
    }
    return std::nullopt;
  }

  auto node_compiler::_verbose_check(
    const executable& exe,
    const managed_node_graph& graph) -> bool
  {
    try {

      auto tp = type_of(exe.object());

      if (!same_type(tp, exe.type())) {
        auto [n, s] = map_obj_to_node(nullptr, {}, graph);
        m_errors.push_back(make_error<compile_error::unexpected_error>(
          n, s, "Verbose type check failed: result type does not match"));
        return false;
      }

      return true;

    } catch (const std::exception& e) {
      auto [n, s] = map_obj_to_node(nullptr, {}, graph);
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        n, s, "Exception thrown while checking result: "s + e.what()));
    } catch (...) {
      auto [n, s] = map_obj_to_node(nullptr, {}, graph);
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        n, s, "Exception thrown while checking result: "));
    }
    return false;
  }

} // namespace yave