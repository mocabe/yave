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
      template <class Property, class Ctor>
      data_type_holder_object_value(
        const object_ptr<Property>& p,
        const object_ptr<Ctor>& c)
        : m_property {p}
        , m_ctor {c}
        , m_data {p->initial_value().clone()}
      {
      }

      data_type_holder_object_value(const data_type_holder_object_value& other)
      {
        m_property = other.m_property;
        m_ctor     = other.m_ctor;
        // clone data (to avoid sharing)
        m_data = other.m_data.clone();
      }

      [[nodiscard]] auto get_data_constructor() -> object_ptr<Object>
      {
        return m_ctor << m_data;
      }

      [[nodiscard]] auto data()
      {
        return m_data;
      }

      [[nodiscard]] auto ctor()
      {
        return m_ctor;
      }

      [[nodiscard]] auto property()
      {
        return m_property;
      }

    private:
      object_ptr<Object> m_property; // property of data
      object_ptr<Object> m_ctor;     // constructor function referring data
      object_ptr<Object> m_data;     // current data
    };

    template <class T>
    struct DataTypeCtor : Function<DataTypeCtor<T>, T, FrameDemand, T>
    {
      auto code() const -> typename DataTypeCtor::return_type
      {
        return DataTypeCtor::template arg<0>();
      }
    };

  } // namespace detail

  /// Data type holder
  using DataTypeHolder = Box<detail::data_type_holder_object_value>;

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
  };

  /// Create new data type holder
  template <class T, class... Args>
  [[nodiscard]] auto make_data_type_holder(Args&&... args)
  {
    using traits        = data_type_property_traits<T>;
    using value_type    = typename traits::value_type;
    using property_type = typename traits::property_type;

    return make_object<DataTypeHolder>(
      make_object<property_type>(std::forward<Args>(args)...),
      make_object<detail::DataTypeCtor<value_type>>());
  }

  /* Some useful property generator */

  namespace detail {

    template <class T>
    struct value_data_type_property
    {
      object_ptr<const T> m_init_value;

      using data_type = typename T::value_type;

      value_data_type_property(data_type val = {})
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

      using data_type = typename T::value_type;

      numeric_data_type_property(
        const data_type& val  = 0,
        const data_type& min  = std::numeric_limits<data_type>::lowest(),
        const data_type& max  = std::numeric_limits<data_type>::max(),
        const data_type& step = 1)
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