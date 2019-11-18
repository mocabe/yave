//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/core/primitive.hpp>
#include <yave/backend/default/render/primitive_constructor.hpp>

namespace yave {

  namespace backend::default_render {

    /// generator to create list of info for each type of primitive
    template <size_t N, class P, class R, class F>
    void primitive_list_gen(R& result, F& func)
    {
      result.emplace_back(
        func(make_primitive<std::variant_alternative_t<N, P>>()));
      if constexpr (N == 0) {
        return;
      } else {
        return primitive_list_gen<N - 1, P>(result, func);
      }
    }

  } // namespace backend::default_render

  template <>
  auto get_primitive_type<backend::tags::default_render>(const primitive_t& v)
    -> object_ptr<const Type>
  {
    return std::visit(
      overloaded {[](const auto& p) {
        using value_type = std::decay_t<decltype(p)>;
        return object_type<
          backend::default_render::PrimitiveConstructor<Box<value_type>>>();
      }},
      v);
  }

  template <>
  auto get_primitive_node_definition<backend::tags::default_render>(
    const primitive_t& v) -> node_definition
  {
    return std::visit(
      overloaded {[&](const auto& p) {
        using value_type = std::decay_t<decltype(p)>;
        return get_node_definition<
          node::PrimitiveConstructor<Box<value_type>>,
          backend::tags::default_render>();
      }},
      v);
  }

  template <>
  auto get_primitive_node_definition_list<backend::tags::default_render>()
    -> std::vector<node_definition>
  {
    std::vector<node_definition> ret;
    backend::default_render::
      primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
        ret, get_primitive_node_definition<backend::tags::default_render>);
    return ret;
  }

} // namespace yave
