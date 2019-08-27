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

  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("node_parser");
      return 1;
    };
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
    if (!tmp)
      return std::nullopt;

    tmp = _desugar(std::move(*tmp));
    if (!tmp)
      return std::nullopt;

    tmp = _parse(std::move(*tmp));
    if (!tmp)
      return std::nullopt;

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

    struct
    {
      void rec(
        const node_graph& g,
        const node_handle& hg,
        node_graph& r, // ref
        const node_handle& hr,
        error_list& e) // ref
      {
        auto inputs = g.input_connections(hg);

        // TODO: Improve performance and prevent stack overflow

        for (auto&& c : inputs) {
          auto c_info   = *g.get_info(c);
          auto src      = c_info.src_node();
          auto src_info = *g.get_info(src);

          Info(g_logger, "Extract node {}({})", src_info.name(), src.id().data);

          // copy src node to `r` and connect to it
          auto child = src_info.is_prim()
                         ? r.add(src_info, *g.get_primitive(src))
                         : r.add(src_info);

          if (!child) {
            e.push_back(
              make_error<parse_error::unexpected_error>("Failed to copy node"));
            throw std::runtime_error("Failed to copy prime tree");
          }

          auto rc =
            r.connect(child, c_info.src_socket(), hr, c_info.dst_socket());

          if (!rc) {
            e.push_back(make_error<parse_error::unexpected_error>(
              "Failed to connect copied nodes"));
            throw std::runtime_error("Failed to copy prime tree");
          }

          // recursively call on child nodes
          rec(g, src, r, child, e);
        }
      }
    } impl;

    node_graph ret;

    // copy root node
    node_handle ret_root;
    {
      auto info = graph.get_info(root);
      assert(info);
      Info(g_logger, "Extract node {}({})", info->name(), root.id().data);
      auto prim = graph.get_primitive(root);
      ret_root  = prim ? ret.add(*info, *prim) : ret.add(*info);
      assert(ret_root);
    }

    impl.rec(graph, root, ret, ret_root, m_errors);

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

    if (!m_errors.empty())
      return {{std::move(graph), root}};

    return std::nullopt;
  }
}