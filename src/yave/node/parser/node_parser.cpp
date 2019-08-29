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
    return *tmp;
  }

  auto node_parser::_extract(const node_graph& graph, const node_handle& root)
    -> std::optional<parsed_node_graph>
  {
    Info(g_logger, "Extracting prime tree from {}", root.id().data);

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

    // collection of relation between old and new nodes.
    std::map<uid, node_handle> n_map;

    // copy nodes
    for (auto&& n : ns) {
      auto info = graph.get_info(n);
      assert(info);

      auto cpy = info->is_prim() ? ret.add(*info, *graph.get_primitive(n))
                                 : ret.add(*info);

      if (!cpy) {
        m_errors.push_back(
          make_error<parse_error::unexpected_error>("Failed to copy node"));
        throw std::runtime_error("Failed to copy prime tree");
      }

      n_map.emplace(n.id(), cpy);

      if (n == root)
        ret_root = cpy;
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

        auto src_itr = n_map.find(src.id());
        auto dst_itr = n_map.find(dst.id());

        if (src_itr == n_map.end() || dst_itr == n_map.end()) {
          m_errors.push_back(make_error<parse_error::unexpected_error>(
            "Could not find copied node"));
          throw std::runtime_error("Failed to copy prime tree");
        }

        auto cpy = ret.connect(
          src_itr->second,
          info->src_socket(),
          dst_itr->second,
          info->dst_socket());

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

    Info(g_logger, "Desugar parsed graph from {}", root.id().data);

    struct
    {
      void rec(node_graph& g, const node_handle& n, error_list& e)
      {
        auto info = g.get_info(n);

        // desugar pass
        {
          if (info == get_node_info<KeyframeInt>()) {
            desugar_KeyframeInt(g, n, e);
          }

          if (info == get_node_info<KeyframeFloat>()) {
            desugar_KeyframeFloat(g, n, e);
          }

          if (info == get_node_info<KeyframeBool>()) {
            desugar_KeyframeBool(g, n, e);
          }
        }

        // traverse
        for (auto&& c : g.input_connections(n)) {
          rec(g, g.get_info(c)->src_node(), e);
        }
      }
    } impl;

    impl.rec(graph, root, m_errors);

    if (m_errors.empty())
      return {{std::move(graph), root}};

    return std::nullopt;
  }


  auto node_parser::_parse(parsed_node_graph&& parsed_graph)
    -> std::optional<parsed_node_graph>
  {
    auto graph = std::move(parsed_graph.graph);
    auto root  = std::move(parsed_graph.root);

    Info(g_logger, "Parse prime tree from {}", root.id().data);

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

    struct
    {
      void rec(const node_graph& g, const node_handle& n, error_list& e)
      {
        auto inputs = g.input_connections(n);

        if (inputs.empty() && !g.is_primitive(n)) {
          e.push_back(make_error<parse_error::no_sufficient_input>(n));
          return;
        }

        for (auto&& c : inputs) {
          auto info = g.get_info(c);
          assert(info);
          rec(g, info->src_node(), e);
        }
      }
    } impl;

    impl.rec(graph, root, m_errors);

    if (m_errors.empty())
      return {{std::move(graph), root}};

    return std::nullopt;
  }
}