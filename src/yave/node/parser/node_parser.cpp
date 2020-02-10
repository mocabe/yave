//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/parser/errors.hpp>
#include <yave/node/core/node_group.hpp>
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

    std::map<node_handle, std::monostate> map;
    std::vector<socket_handle> stack;

    // init stack
    for (auto&& s : clone.output_sockets(clone.root_group())) {
      stack.push_back(s);
    }

    while (!stack.empty()) {

      // socket
      auto os = stack.back();
      stack.pop_back();
      assert(clone.exists(os));

      // node
      auto n = clone.node(os);
      assert(clone.exists(n));

      map.emplace(n, std::monostate {});

      // handle group
      if (clone.is_group(n)) {
        auto ss = clone.get_group_socket_inside(os);
        for (auto&& c : clone.connections(ss)) {
          stack.push_back(clone.get_info(c)->src_socket());
        }
        continue;
      }

      // leave group
      if (clone.is_group_input(n)) {
        auto ss = clone.get_group_socket_outside(os);
        for (auto&& c : clone.connections(ss)) {
          stack.push_back(clone.get_info(c)->src_socket());
        }
        continue;
      }

      assert(!clone.is_group_output(n));

      // normal node
      for (auto&& c : clone.input_connections(n)) {
        stack.push_back(clone.get_info(c)->src_socket());
      }
    }

    // remove unused nodes
    for (auto&& n : clone.nodes()) {
      if (!clone.exists(n))
        continue;
      if (clone.is_group_input(n) || clone.is_group_output(n))
        continue;
      if (map.find(n) != map.end())
        continue;
      clone.destroy(n);
    }

    return {std::move(clone)};
  }

  /// Validate active tree.
  auto node_parser::_validate(managed_node_graph&& graph)
    -> std::optional<managed_node_graph>
  {
    for (auto&& n : graph.nodes()) {

      // root
      if (graph.root_group() == n) {
        // check missing connection for root output
        for (auto&& s : graph.output_sockets(n)) {
          if (graph.connections(graph.get_group_socket_inside(s)).empty()) {
            m_errors.push_back(make_error<parse_error::missing_output>(
              graph.get_group_output(n).id(),
              graph.get_group_socket_inside(s).id()));
          }
        }
        continue;
      }

      // group
      if (graph.is_group(n)) {

        // first pass
        size_t nConnected = 0;
        for (auto&& s : graph.input_sockets(n)) {
          if (!graph.connections(s).empty())
            ++nConnected;
        }

        // second pass
        for (auto&& s : graph.input_sockets(n)) {
          // add vairbale if it's lambda form
          if (nConnected == 0) {
            graph.set_data(s, make_object<Variable>());
          }
          // report missing input
          if (nConnected != 0) {
            if (graph.connections(s).empty()) {
              m_errors.push_back(
                make_error<parse_error::missing_input>(n.id(), s.id()));
            }
          }
        }

        // check missing connection to active output
        for (auto&& s : graph.output_sockets(n)) {
          if (
            !graph.connections(s).empty()
            && graph.connections(graph.get_group_socket_inside(s)).empty()) {
            m_errors.push_back(make_error<parse_error::missing_output>(
              graph.get_group_output(n).id(),
              graph.get_group_socket_inside(s).id()));
          }
        }
        continue;
      }

      // normal
      if (graph.is_group_member(n)) {

        // first pass
        size_t nConnected = 0;
        for (auto&& s : graph.input_sockets(n)) {
          if (!graph.connections(s).empty() || graph.get_data(s))
            ++nConnected;
        }

        // report missing input
        if (nConnected != 0) {
          for (auto&& s : graph.input_sockets(n)) {
            // allow if it has user defined data
            if (graph.connections(s).empty() && !graph.get_data(s)) {
              m_errors.push_back(
                make_error<parse_error::missing_input>(n.id(), s.id()));
            }
          }
        }
        continue;
      }

      if (graph.is_group_input(n) || graph.is_group_output(n)) {
        continue;
      }

      unreachable();
    }

    if (!m_errors.empty())
      return std::nullopt;

    return {std::move(graph)};
  }

} // namespace yave