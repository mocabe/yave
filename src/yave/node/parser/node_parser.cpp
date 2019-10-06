//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/parser/desugar_keyframe.hpp>
#include <yave/node/parser/errors.hpp>
#include <yave/node/obj/keyframe.hpp>
#include <yave/node/obj/constructor.hpp>
#include <yave/support/log.hpp>

namespace {

  // logger
  std::shared_ptr<spdlog::logger> g_logger;

  // init
  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("node_parser");
      return 1;
    }();
  }
} // namespace

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

    tmp = _desugar(std::move(*tmp));
    if (!tmp) {
      Error(g_logger, "Failed to desugar prime tree");
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
      throw std::invalid_argument("Null root node handle");
    }

    if (!graph.exists(root)) {
      m_errors.push_back(
        make_error<parse_error::unexpected_error>("Invalid root handle"));
      throw std::invalid_argument("Invalid root node handle");
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

      auto cpy = info->is_primitive()
                   ? ret.add_with_id(*info, n.id(), *graph.get_primitive(n))
                   : ret.add_with_id(*info, n.id());

      if (!cpy) {
        m_errors.push_back(
          make_error<parse_error::unexpected_error>("Failed to copy node"));
        throw std::runtime_error("Failed to copy prime tree");
      }

      // root node
      if (n == root) {
        ret_root = cpy;
    }
    }

    if (!ret_root) {
      m_errors.push_back(make_error<parse_error::unexpected_error>(
        "Could not find new root node"));
      throw std::runtime_error("Failed to copy prime tree");
    }

    // copy connections
    for (auto&& n : ns) {
      auto cs = graph.input_connections(n);
      for (auto&& c : cs) {
        auto info = graph.get_info(c);
        assert(info);

        auto src = info->src_node();
        auto dst = info->dst_node();

        assert(dst == n);

        auto src_cpy = ret.node(src.id());
        auto dst_cpy = ret.node(dst.id());

        if (!src_cpy || !dst_cpy) {
          m_errors.push_back(make_error<parse_error::unexpected_error>(
            "Could not find copied node"));
          throw std::runtime_error("Failed to copy prime tree");
        }

        auto cpy =
          ret.connect(src_cpy, info->src_socket(), dst_cpy, info->dst_socket());

        if (!cpy) {
          m_errors.push_back(make_error<parse_error::unexpected_error>(
            "Failed to connect copied nodes"));
          throw std::runtime_error("Failed to copy prime tree");
        }
      }
    }

    if (m_errors.empty())
      return {{std::move(ret), ret_root}};

    return std::nullopt;
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
      assert(info);

      // desugar pass
      {
          desugar_KeyframeInt(graph, n, m_errors);
          desugar_KeyframeFloat(graph, n, m_errors);
          desugar_KeyframeBool(graph, n, m_errors);
        }
      }

    // Need to put dummy node if root node can be replaced in desugar pass.
    assert(graph.exists(root));

    if (m_errors.empty())
      return {{std::move(graph), root}};

    return std::nullopt;
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
      throw std::invalid_argument("Null root node handle");
    }

    if (!graph.exists(root)) {
      m_errors.push_back(
        make_error<parse_error::unexpected_error>("Invalid root handle"));
      throw std::invalid_argument("Invalid root node handle");
    }

    auto ns = graph.nodes();

    for (auto&& n : ns) {
      // parse pass
      {
        if (graph.input_connections(n).empty() && !graph.is_primitive(n)) {
          m_errors.push_back(
            make_error<parse_error::no_sufficient_input>(n.id()));
        }
      }
    }

    if (m_errors.empty())
      return {{std::move(graph), root}};

    return std::nullopt;
  }
}