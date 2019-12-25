//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/decl/constructor.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/node/core/function.hpp>
#include <yave/node/core/primitive.hpp>

namespace yave {

  namespace backends::default_common {

    /// Constructor (factory) function for primitive data types
    template <class T>
    struct PrimitiveConstructor : NodeFunction<PrimitiveConstructor<T>, T>
    {
      /// Ctor
      PrimitiveConstructor()
        : m_value {nullptr}
      {
      }

      /// Ctor
      PrimitiveConstructor(object_ptr<const Object> value)
        : m_value {std::move(value)}
      {
      }

      /// code
      typename PrimitiveConstructor::return_type code() const
      {
        return value_cast<T>(m_value);
      }

    private:
      /// value
      object_ptr<const Object> m_value;
    };

    class PrimitiveGetterFunction_X;

    /// Getter function of primitive constructors
    template <class T>
    struct PrimitiveGetterFunction : Function<
                                       PrimitiveGetterFunction<T>,
                                       PrimitiveGetterFunction_X,
                                       PrimitiveConstructor<Box<T>>>
    {
      typename PrimitiveGetterFunction::return_type code() const
      {
        auto obj = PrimitiveGetterFunction::template eval_arg<0>();

        if (same_type(get_type(obj), object_type<Box<T>>()))
          return make_object<PrimitiveConstructor<Box<T>>>(obj);

        throw std::runtime_error(
          "PrimitiveGetterFunction: Invalid type of data");
      }
    };

  } // namespace backends::default_common

  template <class T>
  struct node_definition_traits<
    node::PrimitiveConstructor<T>,
    backend_tags::default_common>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      using value_type = typename T::value_type;
      auto info        = get_node_declaration<node::PrimitiveConstructor<T>>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          backends::default_common::PrimitiveGetterFunction<value_type>>(),
        info.name())};
    }
  };

} // namespace yave
