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

  auto node_parser::parse(const managed_node_graph& graph)
    -> std::optional<managed_node_graph>
  {
    auto lck = _lock();

    std::optional<managed_node_graph> tmp = std::nullopt;

    m_errors.clear();

    tmp = _extract(graph);
    if (!tmp) {
      Error(g_logger, "Failed to parse node graph:");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    tmp = _validate(std::move(*tmp));
    if (!tmp) {
      Error(g_logger, "Failed to parse node graph");
      for (auto&& e : m_errors) {
        Error(g_logger, "error: {}", e.message());
      }
      return std::nullopt;
    }

    Info(g_logger, "Successfully parsed node graph");
    return tmp;
  }

  /// Extract active nodes from managed_node_graph.
  /// First clone entire graph, then omit detached subtrees.
  /// Input graph should have signle output socket, without input sockets.
  /// Currently relying on internal representation of managed_node_graph. Should
  /// fix that later.
  auto node_parser::_extract(const managed_node_graph& graph)
    -> std::optional<managed_node_graph>
  {
    auto root      = graph.root_group();
    auto root_info = graph.get_info(root);

    if (!graph.input_sockets(root).empty()) {
      m_errors.push_back(make_error<parse_error::unexpected_error>(
        "Root group should not have input"));
      return std::nullopt;
    }

    if (graph.output_sockets(root).size() != 1) {
      m_errors.push_back(make_error<parse_error::unexpected_error>(
        "Root group should have single output"));
      return std::nullopt;
    }

    auto clone = graph.clone();
    auto& ng   = clone.node_graph();

    std::map<node_handle, std::monostate> map;
    std::vector<node_handle> stack;

    stack.push_back(
      ng.get_info(clone.output_sockets(clone.root_group())[0])->node());

    // mark valid nodes
    while (!stack.empty()) {

      auto node = stack.back();
      stack.pop_back();

      map.emplace(node, std::monostate {});

      for (auto&& c : ng.input_connections(node)) {

        auto info = ng.get_info(c);

        for (auto&& i : info->src_interfaces()) {
          map.emplace(i, std::monostate {});
        }

        for (auto&& i : info->dst_interfaces()) {
          map.emplace(i, std::monostate {});
        }

        stack.push_back(ng.get_info(c)->src_node());
      }
    }

    // remove orphan trees
    for (auto&& n : clone.nodes()) {

      // already removed
      if (!clone.exists(n))
        continue;
      // io handler
      if (clone.is_group_input(n) || clone.is_group_output(n))
        continue;
      // found
      if (map.find(n) != map.end())
        continue;

      clone.destroy(n);
    }

    return clone;
  }

  /// Validate active tree.
  /// Also relying on internals of managed_node_graph.
  auto node_parser::_validate(managed_node_graph&& graph)
    -> std::optional<managed_node_graph>
  {
    auto& ng = graph.node_graph();

    std::vector<node_handle> stack;
    stack.push_back(
      ng.get_info(graph.output_sockets(graph.root_group())[0])->node());

    while (!stack.empty()) {

      auto node = stack.back();
      stack.pop_back();

      auto is = ng.input_sockets(node);
      auto os = ng.output_sockets(node);

      // no input: ignore
      if (is.empty())
        continue;

      // check input connections
      for (auto&& s : is) {

        auto cs = ng.connections(s);

        if (cs.empty()) {

          // error parameters.
          std::vector<std::pair<node_handle, socket_handle>> es;

          auto interfaces = ng.get_interfaces(s);
          for (auto&& i : interfaces) {
            es.emplace_back(i, s);
          }

          if (es.empty())
            es.emplace_back(node, s);

          for (auto&& [en, es] : es) {
            m_errors.push_back(
              make_error<parse_error::missing_input>(en.id(), es.id()));
          }
        }

        for (auto&& c : cs) {
          stack.push_back(ng.get_info(c)->src_node());
        }
      }
    }

    if (!m_errors.empty())
      return std::nullopt;

    return std::move(graph);
  }

} // namespace yave