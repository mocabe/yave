//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  struct node_argument_object_data;

  /// node default argument data
  using NodeArgument = Box<node_argument_object_data>;

  /// data holder which contains data and property for node arguments.
  struct node_argument_object_data
  {
    node_argument_object_data() = default;

    node_argument_object_data(const node_argument_object_data& other)
    {
      m_property = other.m_property;
      m_ctor     = other.m_ctor;

      // need to clone data (to avoid sharing)
      m_data = other.m_data.clone();
    }

    // FIXME: This is very bad design indeed...
    [[nodiscard]] auto get_data_constructor(
      const object_ptr<const NodeArgument>& arg) -> object_ptr<const Object>
    {
      assert(this == arg.value());
      return m_ctor << arg;
    }

    [[nodiscard]] auto data() const -> object_ptr<const Object>
    {
      return m_data;
    }

    [[nodiscard]] auto ctor() const -> object_ptr<const Object>
    {
      return m_ctor;
    }

    [[nodiscard]] auto property() const -> object_ptr<const Object>
    {
      return m_property;
    }

    void set_property(object_ptr<const Object> new_property)
    {
      assert(!m_property);
      m_property = std::move(new_property);
    }

    void set_ctor(object_ptr<const Object> new_ctor)
    {
      assert(!m_ctor);
      m_ctor = std::move(new_ctor);
    }

    void set_data(object_ptr<const Object> new_data)
    {
      if (m_data)
        assert(same_type(get_type(m_data), get_type(new_data)));
      // atomically update data
      m_data = std::move(new_data);
    }

  private:
    object_ptr<const Object> m_property; // property of data
    object_ptr<const Object> m_ctor;     // constructor function referring data
    object_ptr<const Object> m_data;     // current data
  };

  /// Constructor for node arguments.
  template <class ValueT>
  struct NodeArgumentCtor
    : Function<NodeArgumentCtor<ValueT>, NodeArgument, FrameDemand, ValueT>
  {
    auto code() const -> typename NodeArgumentCtor::return_type
    {
      auto argument = this->template eval_arg<0>();
      return value_cast<ValueT>(argument->data());
    }
  };

  /// Customization point for data type holder property
  template <class T>
  struct node_argument_property_traits
  {
    /// value type of data (for example, Float and Int).
    // using value_type = ...;

    /// propert type of data.
    /// property_type must have `initial_value()` to get initial value_type
    /// object which will be modified by GUI by user.
    // using property_type = ...;
  };

#define YAVE_DECL_NODE_ARGUMENT_PROPERTY(Type, ValueType, PropType) \
  template <>                                                       \
  struct node_argument_property_traits<Type>                        \
  {                                                                 \
    using value_type    = ValueType;                                \
    using property_type = PropType;                                 \
  }

  /// Create new node argument
  template <class T, class... Args>
  [[nodiscard]] auto make_node_argument(Args&&... args)
  {
    using traits        = node_argument_property_traits<T>;
    using value_type    = typename traits::value_type;
    using property_type = typename traits::property_type;

    auto argument = make_object<NodeArgument>();
    auto prop     = make_object<property_type>(std::forward<Args>(args)...);
    auto ctor     = make_object<NodeArgumentCtor<value_type>>();
    auto data     = prop->initial_value().clone();

    argument->set_property(prop);
    argument->set_ctor(ctor);
    argument->set_data(data);

    return argument;
  }

} // namespace yave