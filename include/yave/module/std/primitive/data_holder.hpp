//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/function.hpp>
#include <yave/rts/object_ptr.hpp>

namespace yave {

  namespace detail {

    struct data_type_holder_object_value
    {
      data_type_holder_object_value() = default;

      data_type_holder_object_value(const data_type_holder_object_value& other)
      {
        m_property = other.m_property;
        m_ctor     = other.m_ctor;
        // clone data (to avoid sharing)
        m_data = other.m_data.clone();
      }

      // FIXME: This is very bad design indeed...
      [[nodiscard]] auto get_data_constructor(
        const object_ptr<const Box<data_type_holder_object_value>>& holder)
        -> object_ptr<const Object>
      {
        assert(this == holder.value());
        return m_ctor << holder;
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
      object_ptr<const Object> m_ctor; // constructor function referring data
      object_ptr<const Object> m_data; // current data
    };

  } // namespace detail

  /// Data type holder
  using DataTypeHolder = Box<detail::data_type_holder_object_value>;

  /// Data type constructor
  template <class T>
  struct DataTypeCtor
    : Function<DataTypeCtor<T>, DataTypeHolder, FrameDemand, T>
  {
    auto code() const -> typename DataTypeCtor::return_type
    {
      auto holder = DataTypeCtor::template eval_arg<0>();
      return value_cast<T>(holder->data());
    }
  };

  /// Customization point for data type holder property
  template <class T>
  struct data_type_property_traits
  {
    /// value type of data (for example, Float and Int).
    //using value_type = ...;

    /// propert type of data.
    /// property_type must have `initial_value()` to get initial value_type
    /// object which will be modified by GUI by user.
    //using property_type = ...;
  };

#define YAVE_DECL_DATA_TYPE_PROPERTY(Type, ValueType, PropType) \
  template <>                                                   \
  struct data_type_property_traits<Type>                        \
  {                                                             \
    using value_type    = ValueType;                            \
    using property_type = PropType;                             \
  }

  /// Create new data type holder
  template <class T, class... Args>
  [[nodiscard]] auto make_data_type_holder(Args&&... args)
  {
    using traits        = data_type_property_traits<T>;
    using value_type    = typename traits::value_type;
    using property_type = typename traits::property_type;

    auto holder = make_object<DataTypeHolder>();
    auto prop   = make_object<property_type>(std::forward<Args>(args)...);
    auto ctor   = make_object<DataTypeCtor<value_type>>();
    auto data   = prop->initial_value().clone();

    holder->set_property(prop);
    holder->set_ctor(ctor);
    holder->set_data(data);

    return holder;
  }

  /* Some useful property generator */

  namespace detail {

    template <class T>
    struct value_data_type_property
    {
      object_ptr<const T> m_init_value;

      using value_type = typename T::value_type;

      value_data_type_property(value_type val = {})
        : m_init_value {make_object<T>(std::move(val))}
      {
      }

      auto& initial_value() const
      {
        return m_init_value;
      }
    };
  } // namespace detail

  /// Data type property which contains single initial value
  template <class T>
  using ValueDataTypeProperty = Box<detail::value_data_type_property<T>>;

  namespace detail {

    template <class T>
    struct numeric_data_type_property
    {
      object_ptr<const T> m_init_value;
      object_ptr<const T> m_min, m_max, m_step;

      using value_type = typename T::value_type;

      numeric_data_type_property(
        const value_type& val  = 0,
        const value_type& min  = std::numeric_limits<value_type>::lowest(),
        const value_type& max  = std::numeric_limits<value_type>::max(),
        const value_type& step = 1)
        : m_init_value {make_object<T>(val)}
        , m_min {make_object<T>(min)}
        , m_max {make_object<T>(max)}
        , m_step {make_object<T>(step)}
      {
      }

      auto& initial_value() const
      {
        return m_init_value;
      }

      auto& min() const
      {
        return m_min;
      }

      auto& max() const
      {
        return m_max;
      }

      auto& step() const
      {
        return m_step;
      }
    };
  } // namespace detail

  /// Data type property which contains numeric value properties
  template <class T>
  using NumericDataTypeProperty = Box<detail::numeric_data_type_property<T>>;

} // namespace yave

YAVE_DECL_TYPE(yave::DataTypeHolder, "8cfff49e-b86b-4cca-8d9d-34800003ff3b");