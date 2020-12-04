//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/node/argument.hpp>

namespace yave {

  // ----------------------------------------
  // node_argument_property_node_object_value

  node_argument_property_node_object_value::
    node_argument_property_node_object_value(
      const std::string& name,
      const object_ptr<const Object>& value)
    : m_name {name}
    , m_value {value}
  {
  }

  node_argument_property_node_object_value::
    node_argument_property_node_object_value(
      const std::string& name,
      const object_ptr<const Type>& type,
      const std::vector<object_ptr<NodeArgumentPropNode>>& members)
    : m_name {name}
    , m_stype {type}
    , m_children {members}
  {
  }

  bool node_argument_property_node_object_value::is_value() const
  {
    return static_cast<bool>(m_value);
  }

  bool node_argument_property_node_object_value::is_struct() const
  {
    return !is_value();
  }

  auto node_argument_property_node_object_value::name() const -> std::string
  {
    return m_name;
  }

  auto node_argument_property_node_object_value::value() const
    -> object_ptr<const Object>
  {
    if (is_value())
      return m_value;
    return nullptr;
  }

  void node_argument_property_node_object_value::set_value(
    const object_ptr<const Object> v)
  {
    if (is_value() && same_type(get_type(m_value), get_type(v)))
      m_value = std::move(v);
  }

  auto node_argument_property_node_object_value::type() const
    -> object_ptr<const Type>
  {
    if (is_value())
      return get_type(m_value);
    return m_stype;
  }

  auto node_argument_property_node_object_value::children() const
    -> std::vector<object_ptr<NodeArgumentPropNode>>
  {
    if (!is_value()) {
      assert(!m_children.empty());
      return m_children;
    }
    return {};
  }

  auto node_argument_property_node_object_value::clone() const
    -> object_ptr<NodeArgumentPropNode>
  {
    if (is_value())
      return make_object<NodeArgumentPropNode>(m_name, m_value.clone());

    auto ns = name();
    auto ty = type();
    auto cs = m_children;

    for (auto& c : cs)
      c = c->clone();

    return make_object<NodeArgumentPropNode>(
      std::move(ns), std::move(ty), std::move(cs));
  }

  // ----------------------------------------
  // node_argument_object_value

  node_argument_object_value::node_argument_object_value(
    object_ptr<NodeArgumentPropNode> tree,
    object_ptr<const Object> func)
    : m_tree {std::move(tree)}
    , m_func {std::move(func)}
  {
  }

  auto node_argument_object_value::prop_tree() const
    -> object_ptr<NodeArgumentPropNode>
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