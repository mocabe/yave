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

  namespace backend::default_render {

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
      PrimitiveConstructor(object_ptr<const PrimitiveContainer> value)
        : m_value {std::move(value)}
      {
      }

      /// code
      typename PrimitiveConstructor::return_type code() const
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
                                       PrimitiveConstructor<Box<T>>>
    {
      typename PrimitiveGetterFunction::return_type code() const
      {
        auto container = PrimitiveGetterFunction::template eval_arg<0>();
        auto prim      = container->get();
        if (auto v = std::get_if<T>(&prim)) {
          return make_object<PrimitiveConstructor<Box<T>>>(container);
        } else {
          return make_object<PrimitiveConstructor<Box<T>>>();
        }
      }
    };

  } // namespace backend::default_render

  template <class T>
  struct node_definition_traits<
    node::PrimitiveConstructor<T>,
    backend::tags::default_render>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      using value_type = typename T::value_type;
      auto info        = get_node_declaration<node::PrimitiveConstructor<T>>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          backend::default_render::PrimitiveGetterFunction<value_type>>(),
        info.name())};
    }
  };

} // namespace yave
