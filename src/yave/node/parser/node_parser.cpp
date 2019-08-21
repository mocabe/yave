//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/rts/object_util.hpp>
#include <yave/rts/generalize.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/support/log.hpp>

#include <yave/obj/frame/frame.hpp>

#include <yave/node/support/socket_instance_manager.hpp>

namespace {
  // operator<< for uid
  std::ostream& operator<<(std::ostream& os, const yave::uid& id)
  {
    os << id.data;
    return os;
  }
} // namespace

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace {
  // logger
  std::shared_ptr<spdlog::logger> g_parser_logger;

  // init
  void init_parser_logger()
  {
    [[maybe_unused]] static auto init_logger = []() {
      g_parser_logger = yave::add_logger("node_parser");
      return 1;
    }();
  }
} // namespace

namespace yave {

  node_parser::node_parser(
    const node_graph& graph,
    const bind_info_manager& binds)
    : m_graph {graph}
    , m_binds {binds}
  {
    init_parser_logger();
  }

  std::pair<bool, error_list> node_parser::parse_prime_tree(
    const node_handle& node,
    const std::string& socket) const
  {
    if (!node)
      throw std::invalid_argument("Null node handle");

    if (!m_graph.exists(node))
      throw std::invalid_argument("Invalid node handle");

    Info(
      g_parser_logger,
      "Parse prime tree from node: {}({})#{}",
      m_graph.get_info(node)->name(),
      node.id(),
      socket);

    struct
    {
      void rec(
        const node_graph& graph,
        const node_handle& node,
        error_list& errors)
      {
        auto inputs = graph.input_connections(node);

        // non-primitive node without input connection
        if (inputs.empty() && !graph.is_primitive(node)) {

          errors.push_back(
            make_error<parse_errors::no_sufficient_input>(node));

          Error(
            g_parser_logger,
            "Parse error on prime tree at node {}({})\n"
            "(No sufficient input for non-primitive tree)",
            graph.get_info(node)->name(),
            node.id());

          return;
        }

        for (auto&& c : inputs) {
          auto info = graph.get_info(c);
          assert(info);
          rec(graph, info->src_node(), errors);
        }
      }
    } impl;

    error_list errors;
    impl.rec(m_graph, node, errors);

    if (errors.empty())
      Info(
        "Successfully parsed prime tree from {}({})#{}",
        m_graph.get_info(node)->name(),
        node.id(),
        socket);

    return {errors.empty(), std::move(errors)};
  }

  /*
    Type check prime trees and generate parsed_node_graph.
    Assumes prime tree is already successfully parsed by parse_prime_tree().
    We use socket_instance_manager to collect all node instances attached to
    specific node/socket combination to share instance objects across multiple
    inputs. Instance manager should be given by caller, and it's instance tables
    should be valid for current node graph (otherwise result parsed_node_graph
    will be constructed by invalid instances thus may crash whole program on
    execution).
    Type checking each node is done by following algorithm:
    1. Type check all input sockets.
    2. Find bindings which have exactly same input arguments to current active
    input sockets of the node.
    3. Create genealized type of the node from bindings.
    4. Type check to generalized type, and collect valid bindings.
    5. Decide the single most specialized binding from valid bindings.
   */
  std::pair<std::optional<parsed_node_graph>, error_list>
    node_parser::type_prime_tree(
      const node_handle& node,
      const std::string& socket,
      socket_instance_manager& sim) const
  {
    if (!node)
      throw std::invalid_argument("Null node handle");

    auto binds_lck = m_binds.lock();

    if (!m_graph.exists(node))
      throw std::invalid_argument("Invalid node handle");

    auto node_info = m_graph.get_info(node);
    assert(node_info);

    Info(
      g_parser_logger,
      "Start parse_prime_tree() fron node:{}({})#{}",
      node_info->name(),
      node.id(),
      socket);

    struct
    {
      // Recursive implementation of node type checker.
      // Recursively type check nodes while building return node graph.
      // Returns type of node tree.
      parsed_node_handle rec(
        parsed_node_graph& parsed_graph, /* parsed graph (ref) */
        error_list& errors,              /* error list (ref) */
        socket_instance_manager& sim,    /* instance cache table (ref) */
        const node_graph& graph,         /* graph*/
        const bind_info_manager& binds,  /* bind info */
        const node_handle& node,         /* target tree */
        const std::string& socket)       /* target tree */
      {
        auto node_info      = graph.get_info(node);
        auto node_binds     = binds.get_binds(*node_info);
        const auto& node_os = node_info->output_sockets();
        const auto& node_is = node_info->input_sockets();

        if (std::find(node_os.begin(), node_os.end(), socket) == node_os.end())
          throw std::invalid_argument("Invalid output socket name");

        // list avalable inputs
        std::vector<std::string> inputs;
        for (auto&& is : node_is) {
          if (graph.has_connection(node, is)) {
            inputs.push_back(is);
          }
        }

        if (!node_is.empty())
          assert(!inputs.empty());

        // list of input connections
        std::vector<connection_info> input_connections;
        for (auto&& ic : graph.input_connections(node)) {
          auto info = graph.get_info(ic);
          assert(info);
          input_connections.push_back(*info);
        }

        std::vector<parsed_node_handle> input_handles;

        // recursively parse tree
        for (auto&& i : inputs) {
          for (auto&& ic : input_connections) {
            if (ic.dst_socket() == i) {
              auto handle = rec(
                parsed_graph,
                errors,
                sim,
                graph,
                binds,
                ic.src_node(),
                ic.src_socket());
              input_handles.push_back(handle);
              break;
            }
          }
        }

        // when already in cache
        if (auto inst = sim.find(node, socket)) {

          Info(
            g_parser_logger,
            "Found instance cache at {}({})#{}",
            node_info->name(),
            node.id(),
            socket);

          auto n = parsed_graph.add(inst->instance, inst->type, inst->bind);

          assert(inputs.size() == input_handles.size());

          for (size_t i = 0; i < inputs.size(); ++i) {
            if (!parsed_graph.connect(input_handles[i], n, inputs[i])) {
              throw std::runtime_error("Failed to connect sockets");
            }
          }

          return n;
        }

        std::vector<object_ptr<const Type>> input_types;

        for (auto&& ih : input_handles) {
          input_types.push_back(parsed_graph.get_info(ih)->type());
        }

        // log
        std::string inputs_str;
        for (auto&& i : inputs) inputs_str += fmt::format("{} ", i);
        if (inputs.empty())
          inputs_str = "(no input)";

        Info(
          g_parser_logger,
          "Parsing node in prime tree:\n"
          "| name: {}({})\n"
          "| connected input sockets: {}\n"
          "| output socket: {}",
          node_info->name(),
          node.id(),
          inputs_str,
          socket);

        std::vector<std::shared_ptr<const bind_info>> overloadings;

        // find bindings which are compatible with current input sockets.
        for (auto&& b : node_binds) {
          if (
            b->output_socket() == socket &&
            b->input_sockets().size() == inputs.size()) {
            bool supports_current_inputs = [&] {
              for (auto&& bi : b->input_sockets()) {
                bool bi_in_inputs = [&] {
                  for (auto&& i : inputs) {
                    if (i == bi)
                      return true;
                  }
                  return false;
                }();
                if (!bi_in_inputs)
                  return false;
              }
              return true;
            }();
            if (supports_current_inputs)
              overloadings.push_back(b);
          }
        }

        if (overloadings.empty()) {

          errors.push_back(
            make_error<parse_errors::no_valid_overloading>(node, overloadings));

          Error(
            g_parser_logger,
            "Typing node {}({})#{} failed:\n"
            "| (No overloading for current input connections)",
            node_info->name(),
            node.id(),
            socket);

          for (auto&& ih : input_handles) {
            parsed_graph.remove_subtree(ih);
          }
          // return var
          return parsed_graph.add_dummy();
        }

        // log
        std::string overloadings_str;
        for (auto&& o : overloadings) {
          overloadings_str += "| ";
          for (auto&& i : o->input_sockets())
            overloadings_str += fmt::format("{} ", i);
          if (o->input_sockets().empty())
            overloadings_str += "(no input)";
          overloadings_str += fmt::format("-> {}\n", o->output_socket());
        }
        overloadings_str.pop_back();

        Info(
          g_parser_logger,
          "Overloadings at {}({})#{}:\n"
          "{}",
          node_info->name(),
          node.id(),
          socket,
          overloadings_str);

        // get overloading instances
        std::vector<object_ptr<const Object>> overloading_instances;
        // dummy container for non-primitive nodes
        static object_ptr<PrimitiveContainer> dummy_container =
          make_object<PrimitiveContainer>();

        for (auto&& o : overloadings) {
          if (node_info->is_prim()) {
            auto p = graph.get_primitive_container(node);
            assert(p);
            overloading_instances.push_back(o->get_instance(p));
          } else {
            overloading_instances.push_back(o->get_instance(dummy_container));
          }
        }

        // get overloading types
        std::vector<object_ptr<const Type>> overloading_types;
        for (auto&& ovli : overloading_instances) {
          overloading_types.push_back(get_type(ovli));
        }

        // generalized overloading type
        auto generalized_tp = generalize(overloading_types);

        // log
        std::string input_types_str;
        for (auto&& it : input_types) {
          input_types_str += fmt::format("{}, ", to_string(it));
        }
        if (input_types.empty())
          input_types_str = "(no input type)";

        Info(
          g_parser_logger,
          "Type check at node: {}({})#{}\n"
          "| input types: {}\n"
          "| generalized type: {}",
          node_info->name(),
          node.id(),
          socket,
          input_types_str,
          to_string(generalized_tp));

        // type of target node tree.
        auto node_tp = generalized_tp;
        // updated on each ufinication step
        auto tmp_tp = generalized_tp;

        // Infer type of node on generalized type.
        for (size_t i = 0; i < inputs.size(); ++i) {

          auto& it = input_types[i];

          // Solve constrant for each input type.
          try {

            auto v = genvar();
            // [tmp = it -> v]
            auto c = Constr {tmp_tp, new Type(arrow_type {it, v})};
            // solve constraint
            auto subst = unify({c}, nullptr);
            // update result by substitution.
            node_tp = subst_type_all(subst, node_tp);
            // v contains curried return type.
            tmp_tp = subst_type_all(subst, v);

          } catch (type_error::type_missmatch& e) {

            errors.push_back(make_error<parse_errors::type_missmatch>(
              node, inputs[i], e.expected(), e.provided()));

            Error(
              g_parser_logger,
              "Type check on prime tree failed at {}({})#{}:\n"
              "| on socket: {}\n"
              "| error type: type_missmatch\n"
              "| socket type: {}\n"
              "| expected: {}\n"
              "| provided: {}",
              node_info->name(),
              node.id(),
              socket,
              inputs[i],
              to_string(input_types[i]),
              to_string(e.expected()),
              to_string(e.provided()));

              for(auto&& ih : input_handles) {
                parsed_graph.remove_subtree(ih);
              }

              auto flat = flatten(generalized_tp);
              assert(flat.size() >= 2);
              return parsed_graph.add_dummy(make_object<Type>(
                arrow_type {object_type<Frame>(), flat.back()}));

          } catch (type_error::type_error&) {

            errors.push_back(make_error<parse_errors::no_valid_overloading>(
              node, overloadings));

            Error(
              g_parser_logger,
              "Type check on prime tree failed at {}({})#{}:\n"
              "| on socket: {}\n"
              "| error type: unification failed\n"
              "| socket type: {}",
              node_info->name(),
              node.id(),
              socket,
              inputs[i],
              to_string(input_types[i]));

            for (auto&& ih : input_handles) {
              parsed_graph.remove_subtree(ih);
            }

            return parsed_graph.add_dummy(generalized_tp);
          }
        }

        Info(
          g_parser_logger,
          "Infered type of node: {}({})#{}\n"
          "| node_tp: {}\n"
          "| tmp_tp: {}",
          node_info->name(),
          node.id(),
          socket,
          to_string(node_tp),
          to_string(tmp_tp));

        struct hit_info
        {
          std::shared_ptr<const bind_info> bind;
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
            auto c         = unify({Constr {node_tp, t}}, nullptr);
            auto result_tp = subst_type_all(c, tmp_tp);
            hits.push_back({b, o, t, result_tp});
          } catch (type_error::type_error&) {
            continue;
          }
        }

        if (hits.empty()) {

          errors.push_back(
            make_error<parse_errors::no_valid_overloading>(node, overloadings));

          Info(
            g_parser_logger,
            "Type check on prime tree failed at {}({})#{}:\n"
            "| (No valid overloading found.)",
            node_info->name(),
            node.id(),
            socket);

          for (auto&& ih : input_handles) {
            parsed_graph.remove_subtree(ih);
          }

          return parsed_graph.add_dummy();
        }

        auto ret = hits.front();
        for (size_t i = 1; i < hits.size(); ++i) {
          if (specializable(ret.instance_tp, hits[i].instance_tp)) {
            if (specializable(hits[i].instance_tp, ret.instance_tp)) {

              // TODO: better error info
              errors.push_back(make_error<parse_errors::ambiguous_overloading>(
                node, overloadings));

              Error(
                g_parser_logger,
                "Overloading resolution failed at {}({})#{}:\n"
                "| (Ambiguous overloadings)",
                node_info->name(),
                node.id(),
                socket);

              for (auto&& ih : input_handles) {
                parsed_graph.remove_subtree(ih);
              }

              return parsed_graph.add_dummy(generalized_tp);
            }
            ret = hits[i];
          }
        }

        Info(
          g_parser_logger,
          "Type of node at {}({})#{} in prime tree successfully deduced:\n"
          "| {}",
          node_info->name(),
          node.id(),
          socket,
          to_string(ret.result_tp));

        // create new node in parsed graph
        auto parsed_node =
          parsed_graph.add(ret.instance, ret.result_tp, ret.bind);

        assert(input_handles.size() == ret.bind->input_sockets().size());

        // connect input sockets
        for (size_t i = 0; i < input_handles.size(); ++i) {
          [[maybe_unused]] auto c = parsed_graph.connect(
            input_handles[i], parsed_node, ret.bind->input_sockets()[i]);
        }

        // add instance cache
        sim.add(
          node,
          socket,
          socket_instance {ret.instance, ret.result_tp, ret.bind});

        return parsed_node;
      }

    } impl;

    error_list errors;
    parsed_node_graph parsed_graph;

    auto root =
      impl.rec(parsed_graph, errors, sim, m_graph, m_binds, node, socket);

    // success
    if (errors.empty()) {
      parsed_graph.set_root(root);
      return {std::optional(parsed_graph), std::move(errors)};
    }
    // fail
    return {std::nullopt, std::move(errors)};
  }

  std::unique_lock<std::mutex> node_parser::lock() const
  {
    return std::unique_lock {m_mtx};
  }

} // namespace yave