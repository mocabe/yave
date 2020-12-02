//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/node/core/function.hpp>

#include <yave/data/vector/vector.hpp>
#include <yave/data/string/string.hpp>

namespace yave {

  // fwd
  class node_argument_property_node_object_value;
  using NodeArgumentPropNode = Box<node_argument_property_node_object_value>;

  // name value pair
  struct node_argument_nvp
  {
    data::string name;
    object_ptr<const Object> value;
  };

  /// value node of argument property tree
  class node_argument_property_node_object_value
  {
    /// name of variable
    data::string m_name;

    /// variable which contains value.
    /// possible type: Int, Float, Bool, String and Type.
    /// nullptr when this is struct node
    object_ptr<const Object> m_value;

    /// type of struct.
    /// non-null when this is struct node
    object_ptr<const Type> m_stype;

    /// properties of value
    /// when value.value is nullptr, properties may contain "type" variable
    /// which can be used from GUI otherwise empty.
    data::vector<node_argument_nvp> m_properties;

    /// members of value
    /// this is not empty iff value.value is nullptr.
    data::vector<object_ptr<NodeArgumentPropNode>> m_children;

  public:
    /// construct value node
    node_argument_property_node_object_value(
      const std::string& name,
      const object_ptr<const Object>& value,
      const std::vector<node_argument_nvp>& ps);

    /// construct struct node
    node_argument_property_node_object_value(
      const std::string& name,
      const object_ptr<const Type>& type,
      const std::vector<object_ptr<NodeArgumentPropNode>>& children);

    /// value node?
    bool is_value() const;

    /// struct node?
    bool is_struct() const;

    /// get user defined name
    auto name() const -> std::string;

    /// get value
    /// \returns nullptr when struct node
    auto value() const -> object_ptr<const Object>;

    /// set value
    void set_value(const object_ptr<const Object> v);

    /// get property
    auto property(const std::string& name) const -> object_ptr<const Object>;

    /// get properties
    /// \returns empty vector when value node
    auto properties() const -> std::vector<node_argument_nvp>;

    /// get type
    /// \returns type of contained value when value node, otherwise user
    /// provided type for struct.
    auto type() const -> object_ptr<const Type>;

    /// get members
    /// \returns empty when value node
    auto children() const -> std::vector<object_ptr<NodeArgumentPropNode>>;

    /// clone this node
    auto clone() const -> object_ptr<NodeArgumentPropNode>;
  };

  // fwd
  class node_argument_object_value;
  using NodeArgument = Box<node_argument_object_value>;

  class node_argument_object_value
  {
    /// property tree
    object_ptr<NodeArgumentPropNode> m_tree;

    /// generator function of (NodeArgument -> T)
    object_ptr<const Object> m_func;

  public:
    /// \param tree property variables tree
    /// \param func generator function
    node_argument_object_value(
      object_ptr<NodeArgumentPropNode> tree,
      object_ptr<const Object> func);

    /// get variable tree
    auto prop_tree() const -> object_ptr<NodeArgumentPropNode>;

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
    -> object_ptr<NodeArgumentPropNode>
  {
    return node_argument_traits<T>::create_prop_tree(std::forward<Ts>(args)...);
  }

  /// Get value from argument
  template <class T>
  [[nodiscard]] auto get_node_argument_value(
    const object_ptr<const NodeArgumentPropNode>& arg)
  {
    assert(same_type(arg->type(), object_type<T>()));
    return node_argument_traits<T>::get_value(arg);
  }

  struct node_argument_diff
  {
    /// value node
    object_ptr<NodeArgumentPropNode> node;
    /// new value
    object_ptr<const Object> value;
  };

  /// Get diff from current value.
  /// \returns list of (value node, new value) pairs
  template <class T, class U>
  [[nodiscard]] auto get_node_argument_diff(
    const object_ptr<NodeArgumentPropNode>& arg,
    const U& val) -> std::vector<node_argument_diff>
  {
    assert(same_type(arg->type(), object_type<T>()));
    return node_argument_traits<T>::get_diff(arg, val);
  }

  /// Set value to argument
  /// \param args implementation specific value(s) to set
  template <class T, class... Ts>
  void set_node_argument_value(
    const object_ptr<NodeArgumentPropNode>& arg,
    Ts&&... args)
  {
    assert(same_type(arg->type(), object_type<T>()));
    node_argument_traits<T>::set_value(arg, std::forward<Ts>(args)...);
  }

} // namespace yave