//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/obj/frame.hpp>
#include <yave/node/obj/function.hpp>
#include <yave/node/core/primitive.hpp>
#include <yave/node/core/get_info.hpp>

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

  /// Getter function of primitive constructors
  template <class T>
  struct PrimitiveGetterFunction : Function<
                                     PrimitiveGetterFunction<T>,
                                     PrimitiveContainer,
                                     Constructor<Box<T>>>
  {
    typename PrimitiveGetterFunction::return_type code() const
    {
      auto container = PrimitiveGetterFunction::template eval_arg<0>();
      auto prim      = container->get();
      if (auto v = std::get_if<T>(&prim)) {
        return make_object<Constructor<Box<T>>>(container);
      } else {
        return make_object<Constructor<Box<T>>>();
      }
    }
  };

  // info traits
  template <class T>
  struct node_function_info_traits<Constructor<T>>
  {
    static node_info get_node_info()
    {
      using value_type = typename T::value_type;
      auto name        = get_primitive_name(primitive_t {value_type {}});

      return node_info(name, {}, {"value"});
    }

    static bind_info get_bind_info()
    {
      using value_type = typename T::value_type;
      auto name        = get_primitive_name(primitive_t {value_type {}});

      return bind_info(
        name,
        {},
        "value",
        make_object<PrimitiveGetterFunction<value_type>>(),
        name,
        true);
    }
  };

} // namespace yave