//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/parser/errors.hpp>
#include <yave/node/core/node_group.hpp>
#include <yave/node/decl/constructor.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(node_parser)

namespace yave {

  auto node_parser::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock(m_mtx);
  }

  node_parser::node_parser()
  {
    init_logger();
  }

  auto node_parser::get_errors() const -> error_list
  {
    auto lck = _lock();
    return m_errors.clone();
  }

  auto node_parser::parse(const node_graph& graph, const node_handle& root)
    -> std::optional<parsed_node_graph>
  {
    auto lck = _lock();

    std::optional<parsed_node_graph> tmp = std::nullopt;

    m_errors.clear();

    tmp = _extract(graph, root);
    if (!tmp) {
      Error(g_logger, "Failed to extract prime tree:");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    tmp = _parse(std::move(*tmp));
    if (!tmp) {
      Error(g_logger, "Failed to parse prime tree");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    tmp = _desugar(std::move(*tmp));
    if (!tmp) {
      Error(g_logger, "Failed to desugar prime tree");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    Info(g_logger, "Successfully parsed primary tree in node_graph");
    return tmp;
  }

  auto node_parser::_extract(const node_graph& graph, const node_handle& root)
    -> std::optional<parsed_node_graph>
  {
    Info(g_logger, "Extracting prime tree from {}", to_string(root.id()));

    if (!root) {
      m_errors.push_back(
        make_error<parse_error::unexpected_error>("root handle is null"));
      return std::nullopt;
    }

    if (!graph.exists(root)) {
      m_errors.push_back(
        make_error<parse_error::unexpected_error>("Invalid root handle"));
      return std::nullopt;
    }

    auto ns = std::vector<node_handle> {};

    // list nodes in tree.
    graph.depth_first_search(
      root, [&](const node_handle& n, auto&&) { ns.push_back(n); });

    // result node_graph
    node_graph ret;
    // root of ret
    node_handle ret_root;

    // copy nodes
    for (auto&& n : ns) {

      auto info = graph.get_info(n);
      assert(info);
      assert(!info->is_interface());

      auto cpy = ret.add_copy(graph, n);
      assert(cpy);

      // root node
      if (n == root) {
        ret_root = cpy;
      }
    }

    if (!ret_root) {
      m_errors.push_back(make_error<parse_error::unexpected_error>(
        "Could not find new root node"));
      return std::nullopt;
    }

    // copy connections
    for (auto&& n : ns) {
      auto cs = graph.input_connections(n);
      for (auto&& c : cs) {
        auto info = graph.get_info(c);
        assert(info);

        auto src = info->src_socket();
        auto dst = info->dst_socket();

        assert(info->dst_node() == n);

        auto src_cpy = ret.socket(src.id());
        auto dst_cpy = ret.socket(dst.id());

        if (!src_cpy || !dst_cpy) {
          m_errors.push_back(make_error<parse_error::unexpected_error>(
            "Could not find copied socket"));
          return std::nullopt;
        }

        auto cpy = ret.connect(src_cpy, dst_cpy);

        if (!cpy) {
          m_errors.push_back(make_error<parse_error::unexpected_error>(
            "Failed to connect copied nodes"));
          return std::nullopt;
        }
      }
    }

    if (m_errors.empty())
      return {{std::move(ret), ret_root}};

    return std::nullopt;
  }

  // Remove group IO node
  static void remove_group_io(
    node_graph& graph,
    const node_handle& node,
    const node_info& info)
  {
    if (info.name() != get_node_declaration<node::NodeGroupIOBit>().name())
      return;

    auto ics = graph.input_connections(node);
    auto ocs = graph.output_connections(node);

    if (!ics.empty()) {

      assert(ics.size() == 1);
      auto ic = ics[0];

      auto ici = graph.get_info(ic);
      graph.disconnect(ic);

      [[maybe_unused]] connection_handle c;

      // build connection
      for (auto&& oc : ocs) {
        auto oci = graph.get_info(oc);
        graph.disconnect(oc);
        c = graph.connect(ici->src_socket(), oci->dst_socket());
        assert(c);
      }
    }
  }

  auto node_parser::_desugar(parsed_node_graph&& parsed_graph)
    -> std::optional<parsed_node_graph>
  {
    auto graph = std::move(parsed_graph.graph);
    auto root  = std::move(parsed_graph.root);

    Info(g_logger, "Desugar parsed graph from {}", to_string(root.id()));

    auto ns = graph.nodes();

    for (auto&& n : ns) {

      auto info = graph.get_info(n);

      remove_group_io(graph, n, *info);
    }

    // Need to put dummy node if root node can be replaced in desugar pass.
    assert(graph.exists(root));

    if (m_errors.empty())
      return {{std::move(graph), root}};

    return std::nullopt;
  }

  // check input connections.
  // NOTE: Will remove this with higher-order function supoprt.
  static void check_insufficient_input(
    node_graph& graph,
    error_list& errors,
    const node_handle& node)
  {
    auto iss = graph.input_sockets(node);

    for (auto&& is : iss) {
      if (graph.connections(is).empty())
        errors.push_back(
          make_error<parse_error::no_sufficient_input>(node.id(), is.id()));
    }
  }

  auto node_parser::_parse(parsed_node_graph&& parsed_graph)
    -> std::optional<parsed_node_graph>
  {
    auto graph = std::move(parsed_graph.graph);
    auto root  = std::move(parsed_graph.root);

    Info(g_logger, "Parse prime tree from {}", to_string(root.id()));

    if (!root) {
      m_errors.push_back(
        make_error<parse_error::unexpected_error>("root handle is null"));
      return std::nullopt;
    }

    if (!graph.exists(root)) {
      m_errors.push_back(
        make_error<parse_error::unexpected_error>("Invalid root handle"));
      return std::nullopt;
    }

    auto ns = graph.nodes();

    for (auto&& n : ns) {

      check_insufficient_input(graph, m_errors, n);
    }

    if (m_errors.empty())
      return {{std::move(graph), root}};

    return std::nullopt;
  }
} // namespace yave