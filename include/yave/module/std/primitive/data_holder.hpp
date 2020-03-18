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
      data_type_holder_object_value(object_ptr<Object> d, object_ptr<Object> c)
        : m_data {std::move(d)}
        , m_ctor {std::move(c)}
      {
      }

      data_type_holder_object_value(const data_type_holder_object_value& other)
      {
        // only clone data (to avoid sharing)
        m_data = other.m_data.clone();
        m_ctor = other.m_ctor;
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

    private:
      object_ptr<Object> m_data; // data
      object_ptr<Object> m_ctor; // constructor function referring data
    };

    template <class T>
    struct DataTypeCtor : Function<DataTypeCtor<T>, T, FrameDemand, T>
    {
      auto code() const -> typename DataTypeCtor::return_type
      {
        return DataTypeCtor::template eval_arg<0>();
      }
    };
  } // namespace detail

  using DataTypeHolder = Box<detail::data_type_holder_object_value>;

  template <class T, class... Args>
  [[nodiscard]] auto make_data_type_holder(Args&&... args)
  {
    return make_object<DataTypeHolder>(
      make_object<T>(std::forward<Args>(args)...),
      make_object<detail::DataTypeCtor<T>>());
  }
} // namespace yave

YAVE_DECL_TYPE(yave::DataTypeHolder, "8cfff49e-b86b-4cca-8d9d-34800003ff3b");