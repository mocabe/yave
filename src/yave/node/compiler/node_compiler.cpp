//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/node/compiler/errors.hpp>
#include <yave/core/rts.hpp>

namespace yave {

  Executable node_compiler::compile(
    const parsed_node_graph& graph,
    const parsed_node_handle& root) const
  {
    if (!graph.exists(root))
      throw std::invalid_argument("Does not exists");

    if (![&]() {
          for (auto&& r : graph.roots())
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

    auto apply = impl.rec(graph, root);
    assert(same_type(type_of(apply), graph.get_info(root)->type()));

    return Executable(apply, graph.get_info(root)->type());
  }
} // namespace yave