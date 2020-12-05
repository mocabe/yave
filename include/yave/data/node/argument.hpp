//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/node/core/function.hpp>

#include <yave/data/vector/vector.hpp>
#include <yave/data/string/string.hpp>
#include <yave/obj/property/property.hpp>

namespace yave {

  // fwd
  class node_argument_object_value;
  using NodeArgument = Box<node_argument_object_value>;

  class node_argument_object_value
  {
    /// property tree
    object_ptr<PropertyTreeNode> m_tree;

    /// generator function of (NodeArgument -> T)
    object_ptr<const Object> m_func;

  public:
    /// \param tree property variables tree
    /// \param func generator function
    node_argument_object_value(
      object_ptr<PropertyTreeNode> tree,
      object_ptr<const Object> func);

    /// get variable tree
    auto prop_tree() const -> object_ptr<PropertyTreeNode>;

    /// clone
    auto clone() const -> object_ptr<NodeArgument>;

    /// generate executable object
    auto generate(object_ptr<const NodeArgument> self) const
      -> object_ptr<const Object>;
  };

  /// traits of node argument types
  template <class T>
  struct node_argument_traits
  {
  };

  /// Create new node argument
  template <class T, class... Args>
  [[nodiscard]] auto make_node_argument(Args&&... args)
    -> object_ptr<NodeArgument>
  {
    return node_argument_traits<T>::create(std::forward<Args>(args)...);
  }

  /// Create new node argument property tree
  template <class T, class... Ts>
  [[nodiscard]] auto make_node_argument_prop_tree(Ts&&... args)
    -> object_ptr<PropertyTreeNode>
  {
    return node_argument_traits<T>::create_prop_tree(std::forward<Ts>(args)...);
  }

  /// Get value from argument
  template <class T>
  [[nodiscard]] auto get_node_argument_value(
    const object_ptr<const PropertyTreeNode>& arg)
  {
    assert(same_type(arg->type(), object_type<T>()));
    return node_argument_traits<T>::get_value(arg);
  }

  struct node_argument_diff
  {
    /// value node
    object_ptr<PropertyTreeNode> node;
    /// new value
    object_ptr<const Object> value;
  };

  /// Get diff from current value.
  /// \returns list of (value node, new value) pairs
  template <class T, class U>
  [[nodiscard]] auto get_node_argument_diff(
    const object_ptr<PropertyTreeNode>& arg,
    const U& val) -> std::vector<node_argument_diff>
  {
    assert(same_type(arg->type(), object_type<T>()));
    return node_argument_traits<T>::get_diff(arg, val);
  }

  /// Set value to argument
  /// \param args implementation specific value(s) to set
  template <class T, class... Ts>
  void set_node_argument_value(
    const object_ptr<PropertyTreeNode>& arg,
    Ts&&... args)
  {
    assert(same_type(arg->type(), object_type<T>()));
    node_argument_traits<T>::set_value(arg, std::forward<Ts>(args)...);
  }

} // namespace yave