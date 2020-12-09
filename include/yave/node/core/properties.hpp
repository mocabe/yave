//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/node/argument.hpp>
#include <yave/data/vec/vec.hpp>
#include <yave/node/core/structured_node_graph.hpp>
#include <yave/node/core/node_declaration_store.hpp>

namespace yave {

  /// set argument property
  void set_arg(
    uint64_t idx,
    const object_ptr<NodeArgument>& arg,
    const node_handle& n,
    structured_node_graph& ng);

  /// find sourec argument
  auto get_arg(
    const socket_handle& s,
    const structured_node_graph& ng,
    const node_declaration_map& decls) -> object_ptr<NodeArgument>;

  /// get property
  auto get_arg_property(const socket_handle& s, const structured_node_graph& ng)
    -> object_ptr<PropertyTreeNode>;

  /// set position of node
  void set_pos(
    const glm::vec2& new_pos,
    const node_handle& n,
    structured_node_graph& ng);

  /// get position of node
  auto get_pos(const node_handle& n, const structured_node_graph& ng)
    -> glm::vec2;

} // namespace yave