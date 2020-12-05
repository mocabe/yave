//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/node/argument.hpp>

namespace yave {

  // ----------------------------------------
  // node_argument_object_value

  node_argument_object_value::node_argument_object_value(
    object_ptr<PropertyTreeNode> tree,
    object_ptr<const Object> func)
    : m_tree {std::move(tree)}
    , m_func {std::move(func)}
  {
  }

  auto node_argument_object_value::prop_tree() const
    -> object_ptr<PropertyTreeNode>
  {
    return m_tree;
  }

  auto node_argument_object_value::clone() const -> object_ptr<NodeArgument>
  {
    return make_object<NodeArgument>(m_tree->clone(), m_func);
  }

  auto node_argument_object_value::generate(
    object_ptr<const NodeArgument> self) const -> object_ptr<const Object>
  {
    return m_func << std::move(self);
  }
} // namespace yave