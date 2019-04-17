//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/function.hpp>
#include <yave/obj/frame.hpp>

namespace yave {

  /// Constructor (factory) function for primitive data types
  template <class T>
  struct Constructor : Function<Constructor<T>, Frame::Frame, T>
  {
    /// Ctor
    Constructor()
      : m_value {make_object<T>()}
    {
    }
    /// Ctor
    Constructor(typename T::value_type value)
      : m_value {make_object<T>(value)}
    {
    }
    /// Ctor
    Constructor(object_ptr<const T> value)
      : m_value {std::move(value)}
    {
    }
    /// code
    typename Constructor::return_type code() const
    {
      return m_value;
    }

  private:
    /// value
    object_ptr<const T> m_value;
  };

} // namespace yave