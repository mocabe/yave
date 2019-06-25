//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/obj/frame.hpp>
#include <yave/data/obj/primitive.hpp>
#include <yave/node/obj/function.hpp>

namespace yave {

  /// Constructor (factory) function for primitive data types
  template <class T>
  struct Constructor : NodeFunction<Constructor<T>, T>
  {
    /// Ctor
    Constructor()
      : m_value {nullptr}
    {
    }

    /// Ctor
    Constructor(object_ptr<const PrimitiveContainer> value)
      : m_value {std::move(value)}
    {
    }

    /// code
    typename Constructor::return_type code() const
    {
      if (m_value) {
        auto prim = m_value->get();
        if (auto v = get_if<typename T::value_type>(&prim)) {
          return make_object<T>(*v);
        }
      }
      assert(false);
      return make_object<T>();
    }

  private:
    /// value
    object_ptr<const PrimitiveContainer> m_value;
  };

} // namespace yave