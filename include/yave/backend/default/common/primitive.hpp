//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/core/primitive.hpp>
#include <yave/backend/default/common/primitive_constructor.hpp>

namespace yave {

  namespace backends::default_common {

    /// generator to create list of info for each type of primitive
    template <size_t N, class P, class R, class F>
    void primitive_list_gen(R& result, F& func)
    {
      std::vector defs =
        func(make_primitive<std::variant_alternative_t<N, P>>());

      for (auto&& def : defs)
        result.push_back(std::move(def));

      if constexpr (N == 0)
        return;
      else
        return primitive_list_gen<N - 1, P>(result, func);
    }

  } // namespace backends::default_common

  template <>
  auto get_primitive_type<backend_tags::default_common>(const primitive_t& v)
    -> object_ptr<const Type>
  {
    return std::visit(
      overloaded {[](const auto& p) {
        using value_type = std::decay_t<decltype(p)>;
        return object_type<
          backends::default_common::PrimitiveConstructor<Box<value_type>>>();
      }},
      v);
  }

  template <>
  auto get_primitive_node_definition<backend_tags::default_common>(
    const primitive_t& v) -> std::vector<node_definition>
  {
    return std::visit(
      overloaded {[&](const auto& p) {
        using value_type = std::decay_t<decltype(p)>;
        return get_node_definitions<
          node::PrimitiveConstructor<Box<value_type>>,
          backend_tags::default_common>();
      }},
      v);
  }

  template <>
  auto get_primitive_node_definitions<backend_tags::default_common>()
    -> std::vector<node_definition>
  {
    std::vector<node_definition> ret;
    backends::default_common::
      primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
        ret, get_primitive_node_definition<backend_tags::default_common>);
    return ret;
  }

} // namespace yave
