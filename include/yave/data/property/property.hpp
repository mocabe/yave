//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/node/core/function.hpp>
#include <yave/data/string/string.hpp>
#include <yave/data/vector/vector.hpp>
#include <yave/obj/primitive/primitive.hpp>

#include <concepts>

namespace yave {

  // fwd
  class property_tree_node_object_value;
  using PropertyTreeNode = Box<property_tree_node_object_value>;

  // valid property tree node value type
  template <class T>
  concept TPropertyTreeNodeValue =                 //
    std::same_as<std::remove_const_t<T>, Int> ||   //
    std::same_as<std::remove_const_t<T>, Float> || //
    std::same_as<std::remove_const_t<T>, Bool> ||  //
    std::same_as<std::remove_const_t<T>, String>;

  /// value node of argument property tree
  class property_tree_node_object_value
  {
    /// name of variable
    data::string m_name;

    /// variable which contains value.
    /// possible type: Int, Float, Bool, String and Type.
    /// nullptr when this is struct node
    object_ptr<const Object> m_value;

    /// type of struct.
    /// non-null when this is struct node
    object_ptr<const Type> m_type;

    /// members of value
    /// this is not empty iff value.value is nullptr.
    data::vector<object_ptr<PropertyTreeNode>> m_children;

    struct private_access
    {
    };

  public:
    property_tree_node_object_value(
      private_access,
      const std::string& name,
      const object_ptr<const Object>& value)
      : m_name {name}
      , m_value {value}
    {
    }

    template <TPropertyTreeNodeValue T>
    property_tree_node_object_value(
      const std::string& name,
      const object_ptr<T>& value)
      : m_name {name}
      , m_value {value}
    {
    }

    /// construct struct node
    /// \param name name of node
    /// \param type type of node
    /// \param children child nodes
    property_tree_node_object_value(
      const std::string& name,
      const object_ptr<const Type>& type,
      const std::vector<object_ptr<PropertyTreeNode>>& children)
      : m_name {name}
      , m_type {type}
      , m_children {children}
    {
    }

    /// copy constructor
    property_tree_node_object_value(
      const property_tree_node_object_value& other)
      : m_name {other.m_name}
      , m_value {other.m_value}
      , m_type {other.m_type}
      , m_children {other.m_children}
    {
      for (auto&& c : m_children)
        c = c.clone();
    }

    /// value node?
    bool is_value() const
    {
      return static_cast<bool>(m_value);
    }

    /// get user defined name
    auto name() const -> std::string
    {
      return m_name;
    }

    /// get value
    /// \requires is_value()
    template <TPropertyTreeNodeValue T>
    auto get_value() const -> object_ptr<const T>
    {
      assert(is_value());
      return value_cast<const T>(m_value);
    }

    /// safe set value
    /// \requires is_value()
    template <TPropertyTreeNodeValue T>
    void set_value(object_ptr<T> v)
    {
      assert(is_value() && same_type(get_type(m_value), get_type(v)));
      m_value = std::move(v);
    }

    /// unsafe version of get_value.
    /// avoid using this as much as possible.
    auto get_value_untyped() const -> object_ptr<const Object>
    {
      assert(is_value());
      return m_value;
    }

    /// unsafe version of set_value.
    /// avoid using this as much as possible.
    void set_value_untyped(object_ptr<const Object> v)
    {
      assert(is_value() && same_type(get_type(m_value), get_type(v)));
      m_value = std::move(v);
    }

    /// get type
    /// \requires !is_value()
    auto type() const -> object_ptr<const Type>
    {
      assert(!is_value());
      return m_type;
    }

    /// get members
    /// \requires !is_value()
    auto children() const -> std::vector<object_ptr<PropertyTreeNode>>
    {
      assert(!is_value());
      return m_children;
    }
  };
} // namespace yave