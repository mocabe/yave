//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/core/rts/object_util.hpp>
#include <yave/core/rts/generalize.hpp>
#include <yave/core/rts/to_string.hpp>
#include <yave/support/log.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <ostream>

namespace {

  /// logger
  std::shared_ptr<spdlog::logger> g_parser_logger;

  /// operator<< for uid
  std::ostream& operator<<(std::ostream& os, const yave::uid& id)
  {
    os << id.data;
    return os;
  }

} // namespace

namespace yave {

  node_parser::node_parser(
    const node_graph& graph,
    const bind_info_manager& binds)
    : m_graph {graph}
    , m_binds {binds}
  {
    [[maybe_unused]] static auto init_logger = []() {
      g_parser_logger = add_logger("node_parser");
      return 1;
    }();
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
      object_ptr<const Type> rec(
        error_list& errors,
        const node_graph& graph,
        const bind_info_manager& binds,
        const node_handle& node,
        const std::string& socket)
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
          if (b->output_socket() == socket) {
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

        // log
        std::string overloadings_str;
        for (auto&& o : overloadings) {
          overloadings_str += "| ";
          for (auto&& i : o->input_sockets())
            overloadings_str += fmt::format("{} ", i);
          overloadings_str += "\n";
        }

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
        for (auto&& o : overloadings) {
          primitive_t prim = {};
          if (node_info->is_prim()) {
            auto p = graph.get_primitive(node);
            assert(p);
            prim = *p;
          }
          overloading_instances.push_back(o->get_instance(prim));
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

        auto node_tp = generalized_tp;
        auto tmp_tp  = generalized_tp;

        // Infer type of node on generalized type
        for (auto&& it : input_types) {
          auto v = genvar();
          auto c = std::vector {Constr {tmp_tp, new Type(arrow_type {it, v})}};
          auto subst = unify(c, nullptr);
          node_tp    = subst_type_all(subst, node_tp);
          tmp_tp     = subst_type_all(subst, v);
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
          return genvar();
        }

        auto ret = hits.front();
        for (size_t i = 0; i < hits.size(); ++i) {
          if (specializable(ret.instance_tp, hits[i].instance_tp)) {
            if (specializable(hits[i].instance_tp, ret.instance_tp)) {
              errors.push_back(make_error<parse_errors::ambiguous_overloading>(
                node, overloadings));
              return genvar();
            }
            ret = hits[i];
          }
        }
        return ret.result_tp;
      }

    } parse_prime_node;

    return parse_prime_node.rec(errors, m_graph, m_binds, node, socket);
  }

} // namespace yave