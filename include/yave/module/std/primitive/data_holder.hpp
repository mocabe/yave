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
        , m_data {p->initial_value()}
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

    struct data_type_holder_property_value
    {
      /// initial value of data
      object_ptr<const Object> m_init_value;

      template <class T>
      data_type_holder_property_value(object_ptr<T> init_value)
        : m_init_value {std::move(init_value)}
      {
      }

      data_type_holder_property_value(
        const data_type_holder_property_value& other) = default;

      auto initial_value() const
      {
        return m_init_value.clone();
      }
    };

    // default data type property
    using DataTypeHolderProperty = Box<data_type_holder_property_value>;

  } // namespace detail

  /// Customize point for data type holder property object
  template <class T>
  struct data_type_holder_traits
  {
    template <class... Args>
    static auto make_property_object(Args&&... args)
    {
      // create default property
      return make_object<detail::DataTypeHolderProperty>(
        make_object<T>(std::forward<Args>(args)...));
    }
  };

  /// Data type holder
  using DataTypeHolder = Box<detail::data_type_holder_object_value>;

  /// Create new data type holder
  template <class T, class... Args>
  [[nodiscard]] auto make_data_type_holder(Args&&... args)
  {
    return make_object<DataTypeHolder>(
      data_type_holder_traits<T>::make_property_object(
        std::forward<Args>(args)...),
      make_object<detail::DataTypeCtor<T>>());
  }
} // namespace yave

YAVE_DECL_TYPE(yave::DataTypeHolder, "8cfff49e-b86b-4cca-8d9d-34800003ff3b");

YAVE_DECL_TYPE(
  yave::detail::DataTypeHolderProperty,
  "db9db89f-9588-4332-8946-790f39ed282a");