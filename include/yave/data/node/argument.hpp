//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  /// abstract node argument interface
  class node_argument_holder
  {
  public:
    virtual ~node_argument_holder() noexcept = default;

    /// get current data
    virtual auto data() const -> object_ptr<const Object> = 0;
    /// get current property
    virtual auto property() const -> object_ptr<const Object> = 0;
    /// create runtime representation
    virtual auto on_compile() const -> object_ptr<const Object> = 0;

    /// set new data
    virtual void set_data(object_ptr<const Object> new_data) = 0;
    /// clone holder
    virtual auto clone() -> std::unique_ptr<node_argument_holder> = 0;
  };

  /// data holder which contains data and property for node arguments.
  class node_argument_object_data
  {
    std::unique_ptr<node_argument_holder> m_holder;

  public:
    node_argument_object_data(std::unique_ptr<node_argument_holder> holder)
      : m_holder {std::move(holder)}
    {
    }

    node_argument_object_data(const node_argument_object_data& other)
      : m_holder {other.m_holder->clone()}
    {
    }

    /// get current data
    auto data() const
    {
      return m_holder->data();
    }

    /// get property
    auto property() const
    {
      return m_holder->property();
    }

    /// set new data
    void set_data(object_ptr<const Object> new_data)
    {
      return m_holder->set_data(std::move(new_data));
    }

    /// get object
    auto on_compile() const
    {
      return m_holder->on_compile();
    }
  };

  /// node default argument data
  using NodeArgument = Box<node_argument_object_data>;

  /// Customization point for data type holder property
  template <class T>
  struct node_argument_property_traits
  {
    // value type of data (for example, Float and Int).
    // using value_type = ...;

    // propert type of data.
    // property_type must have `initial_value()` to get initial value_type
    // object which will be modified by GUI by user.
    // using property_type = ...;
  };

#define YAVE_DECL_NODE_ARGUMENT_PROPERTY(Type, ValueType, PropType) \
  template <>                                                       \
  struct node_argument_property_traits<Type>                        \
  {                                                                 \
    using value_type    = ValueType;                                \
    using property_type = PropType;                                 \
  }

  namespace detail {

    struct node_argument_ctor_arg
    {
      object_ptr<const Object> data;
    };

    // internal
    using NodeArgumentCtorArg = Box<node_argument_ctor_arg>;

    // Constructor for node arguments.
    template <class ValueT>
    struct NodeArgumentCtor : Function<
                                NodeArgumentCtor<ValueT>,
                                NodeArgumentCtorArg,
                                FrameDemand,
                                ValueT>
    {
      auto code() const -> typename NodeArgumentCtor::return_type
      {
        auto argument = this->template eval_arg<0>();
        return value_cast<ValueT>(argument->data);
      }
    };

    // node argument which generate data constructor
    template <class T>
    class data_node_argument : public node_argument_holder
    {
      // property of data
      const object_ptr<const Object> m_property;
      // constructor func
      const object_ptr<const Object> m_ctor;
      // current data
      object_ptr<const Object> m_data;

    public:
      data_node_argument(
        object_ptr<const Object> prop,
        object_ptr<const Object> ctor,
        object_ptr<const Object> data)
        : m_property {std::move(prop)}
        , m_ctor {std::move(ctor)}
        , m_data {std::move(data)}
      {
      }

      data_node_argument(const data_node_argument&) = delete;

      void set_data(object_ptr<const Object> new_data) override
      {
        assert(m_data);
        assert(same_type(get_type(m_data), get_type(new_data)));
        // atomically update data
        m_data = std::move(new_data);
      }

      auto data() const -> object_ptr<const Object> override
      {
        return m_data;
      }

      auto property() const -> object_ptr<const Object> override
      {
        return m_property;
      }

      auto clone() -> std::unique_ptr<node_argument_holder> override
      {
        return std::make_unique<data_node_argument>(
          m_property, m_ctor, m_data.clone());
      }

      auto on_compile() const -> object_ptr<const Object> override
      {
        return m_ctor << make_object<NodeArgumentCtorArg>(m_data);
      }
    };
  } // namespace detail

  /// Create new node argument
  template <class T, class... Args>
  [[nodiscard]] auto make_node_argument(Args&&... args)
  {
    using traits        = node_argument_property_traits<T>;
    using value_type    = typename traits::value_type;
    using property_type = typename traits::property_type;

    auto prop = make_object<property_type>(std::forward<Args>(args)...);
    auto ctor = make_object<detail::NodeArgumentCtor<value_type>>();

    return make_object<NodeArgument>(
      std::make_unique<detail::data_node_argument<T>>(
        prop, ctor, prop->initial_value().clone()));
  }

} // namespace yave

YAVE_DECL_TYPE(
  yave::detail::NodeArgumentCtorArg,
  "e4060e95-ed9e-4610-98ba-97823c09e3f5");