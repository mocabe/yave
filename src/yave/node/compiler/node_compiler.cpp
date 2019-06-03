//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/core/rts.hpp>

namespace yave {

  node_compiler::node_compiler(const parsed_node_graph& parsed_graph)
    : m_parsed_graph {parsed_graph}
  {
  }

  executable node_compiler::compile(const parsed_node_handle& root) const
  {
    if (!m_parsed_graph.exists(root))
      throw std::invalid_argument("Does not exists");

    if (![&]() {
          for (auto&& r : m_parsed_graph.roots())
            if (r == root)
              return true;
          return false;
        }())
      throw std::invalid_argument("Invalid root node specified");

    struct
    {
      object_ptr<const Object>
        rec(const parsed_node_graph& graph, const parsed_node_handle& node)
      {
        assert(graph.exists(node));

        auto node_info = *graph.get_info(node);

        // instance of this node
        auto obj = node_info.instance();

        // apply arguments
        for (auto&& ic : graph.input_connections(node)) {
          obj = obj << rec(graph, graph.get_info(ic)->src_node());
        }

        return obj;
      }
    } impl;

    auto apply = impl.rec(m_parsed_graph, root);
    assert(same_type(type_of(apply), m_parsed_graph.get_info(root)->type()));

    return executable(apply, m_parsed_graph.get_info(root)->type());
  }

  std::unique_lock<std::mutex> node_compiler::lock() const
  {
    return std::unique_lock {m_mtx};
  }
} // namespace yave