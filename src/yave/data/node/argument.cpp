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

  node_argument_object_value::node_argument_object_value(
    const node_argument_object_value& other)
    : m_tree {other.m_tree.clone()}
    , m_func {other.m_func}
  {
  }

  auto node_argument_object_value::property() const
    -> object_ptr<PropertyTreeNode>
  {
    return m_tree;
  }

  auto node_argument_object_value::generator() const -> object_ptr<const Object>
  {
    return m_func;
  }

  auto node_argument_object_value::generate(
    object_ptr<const NodeArgument> self) const -> object_ptr<const Object>
  {
    return m_func << std::move(self);
  }
} // namespace yave