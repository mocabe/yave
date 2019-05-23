//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/core/rts/object_util.hpp>
#include <yave/core/rts/generalize.hpp>
#include <yave/core/rts/to_string.hpp>
#include <yave/support/log.hpp>

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

  object_ptr<const Type> node_parser::type_prime_tree(
    const node_handle& node,
    const std::string& socket,
    error_list& errors) const
  {
    if (!node)
      throw std::invalid_argument("Null node handle");

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
      // Returns type of node tree.
      object_ptr<const Type> rec(
        error_list& errors,             /* error list (ref) */
        const node_graph& graph,        /* graph*/
        const bind_info_manager& binds, /* bind info */
        const node_handle& node,        /* target tree */
        const std::string& socket)      /* target tree */
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

        // recursively parse tree
        std::vector<object_ptr<const Type>> input_types;

        for (auto&& i : inputs) {
          for (auto&& ic : input_connections) {
            if (ic.dst_socket() == i) {
              input_types.push_back(
                rec(errors, graph, binds, ic.src_node(), ic.src_socket()));
              break;
            }
          }
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

          return genvar();
        }

        // log
        std::string overloadings_str;
        for (auto&& o : overloadings) {
          overloadings_str += "| ";
          for (auto&& i : o->input_sockets())
            overloadings_str += fmt::format("{}", i);
          if (o->input_sockets().empty())
            overloadings_str += "(no input)";
          overloadings_str += fmt::format("->{}\n", o->output_socket());
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

            return genvar();
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

            return genvar();
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
          const bind_info* bind;
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
            hits.push_back({b.get(), o, t, result_tp});
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

          return genvar();
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

              return genvar();
            }
            ret = hits[i];
          }
        }

        Info(
          g_parser_logger,
          "Type of node {}({})#{} successfully deduced:\n"
          "| {}",
          node_info->name(),
          node.id(),
          socket,
          to_string(ret.result_tp));

        return ret.result_tp;
      }

    } parse_prime_node;

    return parse_prime_node.rec(errors, m_graph, m_binds, node, socket);
  }

} // namespace yave