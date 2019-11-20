//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/primitive.hpp>
#include <yave/rts/eval.hpp>
#include <yave/obj/string/string.hpp>
#include <yave/node/class/constructor.hpp>

namespace yave {

  namespace {
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
  } // namespace

  auto get_primitive_name_list() -> std::vector<std::string>
  {
    std::vector<std::string> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_name);
    return ret;
  }

  auto get_primitive_node_declaration(const primitive_t& v) -> node_declaration
  {
    return std::visit(
      overloaded {[&](const auto& p) {
        using value_type = std::decay_t<decltype(p)>;
        return get_node_declaration<
          node::PrimitiveConstructor<Box<value_type>>>();
      }},
      v);
  }

  auto get_primitive_node_declaration_list() -> std::vector<node_declaration>
  {
    std::vector<node_declaration> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_node_declaration);
    return ret;
  }

} // namespace yave