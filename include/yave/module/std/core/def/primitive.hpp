//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/decl/primitive.hpp>
#include <yave/obj/behaviour/frame_demand.hpp>

namespace yave {

  namespace modules::_std::core {

    template <class T>
    struct PrimitiveConstructor : Function<PrimitiveConstructor<T>, FrameDemand, T>
    {
      PrimitiveConstructor(object_ptr<const T> value)
        : m_value {std::move(value)}
      {
      }

      auto code() const -> typename PrimitiveConstructor::return_type
      {
        return m_value;
      }

    private:
      object_ptr<const T> m_value;
    };

    class PrimitiveGetterFunction_X;

    template <class T>
    struct PrimitiveGetterFunction : Function<
                                       PrimitiveGetterFunction<T>,
                                       PrimitiveGetterFunction_X,
                                       PrimitiveConstructor<T>>
    {
      auto code() const -> typename PrimitiveGetterFunction<T>::return_type
      {
        auto arg = PrimitiveGetterFunction::template eval_arg<0>();
        return make_object<PrimitiveConstructor<T>>(
          value_cast<const T>(object_ptr<const Object>(arg)));
      }
    };

  } // namespace modules::_std::core

  template <class T>
  struct node_definition_traits<
    node::PrimitiveConstructor<T>,
    modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::PrimitiveConstructor<T>>();

      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<modules::_std::core::PrimitiveGetterFunction<T>>(),
        info.name())};
    }
  };

} // namespace yave