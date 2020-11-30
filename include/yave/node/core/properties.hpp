//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/node/argument.hpp>
#include <yave/data/vec/vec.hpp>
#include <yave/node/core/structured_node_graph.hpp>

namespace yave {

  /// set argument property
  void set_arg(
    structured_node_graph& ng,
    socket_handle s,
    object_ptr<NodeArgument> holder);

  /// get argument property
  auto get_arg(const structured_node_graph& ng, socket_handle s)
    -> object_ptr<NodeArgument>;

  /// set position of node
  void set_pos(structured_node_graph& ng, node_handle n, glm::vec2 new_pos);

  /// get position of node
  auto get_pos(const structured_node_graph& ng, node_handle n) -> glm::vec2;

} // namespace yave