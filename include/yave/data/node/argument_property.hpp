//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/node/argument.hpp>

namespace yave::data {

  /// argument property for value types
  template <class T>
  struct value_argument_property_object_data
  {
    object_ptr<const T> m_init_value;

    using value_type = typename T::value_type;

    value_argument_property_object_data(value_type val = {})
      : m_init_value {make_object<T>(std::move(val))}
    {
    }

    auto& initial_value() const
    {
      return m_init_value;
    }
  };

  /// argument property for numeric types
  template <class T>
  struct numeric_argument_property_object_data
  {
    object_ptr<const T> m_init_value;
    object_ptr<const T> m_min, m_max, m_step;

    using value_type = typename T::value_type;

    numeric_argument_property_object_data(
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

} // namespace yave::data