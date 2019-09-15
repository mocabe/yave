//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/support/log.hpp>
#include <yave/node/support/socket_instance_manager.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/rts/generalize.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/rts/value_cast.hpp>

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
    const bind_info_manager& bim) -> std::optional<executable>
  {
    auto lck = _lock();

    m_errors.clear();

    auto ng = _optimize(std::move(parsed_graph));
    if (!ng) {
      Error(g_logger, "Failed to optmize parsed node graph");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    auto exe = _generate(*ng, bim);
    if (!exe) {
      Error(g_logger, "Failed to generate apply graph");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    auto succ = _verbose_check(*exe);
    if (!succ) {
      Error(g_logger, "Verbose check failed");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    return exe;
  }

  auto node_compiler::_optimize(parsed_node_graph&& parsed_graph)
    -> std::optional<parsed_node_graph>
  {
    // TODO...
    return std::move(parsed_graph);
  }

  /*
    Type check prime trees and generate apply graph.
    Assumes prime tree is already successfully parsed by node_parser.
    We use socket_instance_manager to collect all node instances attached to
    specific node/socket combination to share instance objects across multiple
    inputs. 

    Type checking each node is done by following algorithm:
    1. Type check all input sockets.
    2. Find bindings which have exactly same input arguments to current active
    input sockets of the node.
    3. Create genealized type of the node from bindings.
    4. Type check to generalized type, and collect valid bindings.
    5. Decide the single most specialized binding from valid bindings.
   */
  auto node_compiler::_generate(
    const parsed_node_graph& parsed_graph,
    const bind_info_manager& bim) -> std::optional<executable>
  {
    struct
    {
      // Recursive implementation of node type checker.
      socket_instance rec(
        const node_handle& node,      /* target node */
        const std::string& socket,    /* target socket */
        const node_graph& graph,      /* graph */
        const bind_info_manager& bim, /* bind info */
        socket_instance_manager& sim, /* instance cache table (ref) */
        error_list& errors)           /* error list (ref) */
      {
        auto node_info      = graph.get_info(node);
        auto node_binds     = bim.get_binds(*node_info);
        const auto& node_os = node_info->output_sockets();
        const auto& node_is = node_info->input_sockets();

        // when already in cache
        if (auto inst = sim.find(node, socket)) {

          Info(
            g_logger,
            "Found instance cache at {}({})#{}",
            node_info->name(),
            to_string(node.id()),
            socket);

          return *inst;
        }

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

        std::vector<socket_instance> input_instances;

        // recursively parse tree
        for (auto&& i : inputs) {
          for (auto&& ic : input_connections) {
            if (ic.dst_socket() == i) {
              input_instances.push_back(
                rec(ic.src_node(), ic.src_socket(), graph, bim, sim, errors));
              break;
            }
          }
        } 

        // input type list (for convenient)
        std::vector<object_ptr<const Type>> input_types;

        for (auto&& inst : input_instances) {
          input_types.push_back(inst.type);
        }

        // log
        {
          std::string inputs_str;
          for (auto&& i : inputs) inputs_str += fmt::format("{} ", i);
          if (inputs.empty())
            inputs_str = "(no input)";

          Info(g_logger, "[ Type checking node: id={} ]", to_string(node.id()));
          Info(g_logger, "- name: {}", node_info->name());
          Info(g_logger, "- connected input sockets: {}", inputs_str);
          Info(g_logger, "- output socket: {}", socket);
          Info(g_logger, "- registered overloadings: {}", node_binds.size());
        }

        std::vector<std::shared_ptr<const bind_info>> overloadings;

        // find bindings which are compatible with current input sockets.
        for (auto&& b : node_binds) {

          auto bis = b->input_sockets();
          auto bos = b->output_socket();

          if (bos != socket) {
            continue;
          }

          if (bis.size() != inputs.size())
            continue;

          auto match = [&]() {
            for (size_t i = 0; i < inputs.size(); ++i) {
              if (bis[i] != inputs[i]) {
                return false;
              }
            }
            return true;
          }();

          if (!match)
            continue;

          overloadings.push_back(b);
        }

        if (overloadings.empty()) {

          errors.push_back(make_error<compile_error::no_valid_overloading>(
            node, overloadings));

          Error(
            g_logger,
            "[ Typing node {}({})#{} failed ]",
            node_info->name(),
            to_string(node.id()),
            socket);
          Error(g_logger, "- (No overloading for current input connections)");

          return {nullptr, object_type<Undefined>(), nullptr};
        }

        // log
        {
          Info(
            g_logger,
            "[ Overloadings at {}({})#{} ]",
            node_info->name(),
            to_string(node.id()),
            socket);

          for (auto&& o : overloadings) {

            std::string str;
            for (auto&& i : o->input_sockets()) str += fmt::format("{} ", i);
            if (o->input_sockets().empty())
              str += "(no input)";
            str += fmt::format("-> {}", o->output_socket());

            Info(g_logger, "- {}", str);
          }
        }

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
            socket);
          Info(g_logger, "- input types: {}", input_types_str);
          Info(g_logger, "- generalized type: {}", to_string(generalized_tp));
        }

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

            errors.push_back(make_error<compile_error::type_missmatch>(
              node, inputs[i], e.expected(), e.provided()));

            {
              Error(
                g_logger,
                "[ Type check on prime tree failed at {}({})#{} ]",
                node_info->name(),
                to_string(node.id()),
                socket);
              Error(g_logger, "- on socket: {}", inputs[i]);
              Error(g_logger, "- error type: type_missmatch");
              Error(g_logger, "- socket type: {}", to_string(input_types[i]));
              Error(g_logger, "- expected: {}", to_string(e.expected()));
              Error(g_logger, "- provided: {}", to_string(e.provided()));
            }

            auto flat = flatten(generalized_tp);
            assert(flat.size() >= 2);

            return {nullptr, flat.back(), nullptr};

          } catch (type_error::type_error&) {

            errors.push_back(make_error<compile_error::no_valid_overloading>(
              node, overloadings));

            {
              Error(
                g_logger,
                "[ Type check on prime tree failed at {}({})#{} ]",
                node_info->name(),
                to_string(node.id()),
                socket);
              Error(g_logger, "- on socket: {}", inputs[i]);
              Error(g_logger, "- error type: unification failed");
              Error(g_logger, "- socket type: {}", to_string(input_types[i]));
            }

            return {nullptr, generalized_tp, nullptr};
          }
        }

        {
          Info(
            g_logger,
            "[ Infered type of node: {}({})#{} ]",
            node_info->name(),
            to_string(node.id()),
            socket);
          Info(g_logger, "- node_tp: {}", to_string(node_tp));
          Info(g_logger, "- tmp_tp: {}", to_string(tmp_tp));
        }

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

          errors.push_back(make_error<compile_error::no_valid_overloading>(
            node, overloadings));

          {
            Info(
              g_logger,
              "[ Type check on prime tree failed at {}({})#{} ]",
              node_info->name(),
              to_string(node.id()),
              socket);
            Info(g_logger, "- (No valid overloading found.)");
          }

          return {nullptr, object_type<Undefined>(), nullptr};
        }

        auto ret = hits.front();
        for (size_t i = 1; i < hits.size(); ++i) {
          if (specializable(ret.instance_tp, hits[i].instance_tp)) {
            if (specializable(hits[i].instance_tp, ret.instance_tp)) {

              // TODO: better error info
              errors.push_back(make_error<compile_error::ambiguous_overloading>(
                node, overloadings));

              {
                Error(
                  g_logger,
                  "[ Overloading resolution failed at {}({})#{} ]",
                  node_info->name(),
                  to_string(node.id()),
                  socket);
                Error(g_logger, "- (Ambiguous overloadings)");
              }

              return {nullptr, object_type<Undefined>(), nullptr};
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
            socket);
          Info(g_logger, "- {}", to_string(ret.result_tp));
        }

        // add instance cache

        sim.add(
          node,
          socket,
          socket_instance {ret.instance, ret.result_tp, ret.bind});

        return {ret.instance, ret.result_tp, ret.bind};
      }
    } impl;

    auto& graph      = parsed_graph.graph;
    auto& root_node  = parsed_graph.root;
    auto root_info   = *graph.get_info(root_node);
    auto root_socket = root_info.output_sockets()[0];
    auto sim         = socket_instance_manager();

    auto root_instance =
      impl.rec(root_node, root_socket, graph, bim, sim, m_errors);

    // success
    if (m_errors.empty()) {
      return {{root_instance.instance, root_instance.type}};
    }
    // fail
    return std::nullopt;
  }

  auto node_compiler::_verbose_check(const executable& exe) -> bool
  {
    // TODO: better check and output.
    try {
      return static_cast<bool>(type_of(exe.object()));
    } catch (...) {
      m_errors.push_back(make_error<compile_error::unexpected_error>(
        "Verbose type check failed"));
      return false;
    }
  }

} // namespace yave