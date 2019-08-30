//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/primitive.hpp>
#include <yave/node/obj/constructor.hpp>
#include <yave/rts/eval.hpp>
#include <yave/obj/string/string.hpp>

namespace yave {

  node_info get_primitive_node_info(const primitive_t& v)
  {
    return std::visit(
      overloaded {[&](const auto& p) {
        using value_type = std::decay_t<decltype(p)>;
        return get_node_info<node::PrimitiveConstructor<Box<value_type>>>();
      }},
      v);
  }

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

  std::vector<std::string> get_primitive_name_list()
  {
    std::vector<std::string> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_name);
    return ret;
  }

  std::vector<node_info> get_primitive_node_info_list()
  {
    std::vector<node_info> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_node_info);
    return ret;
  }

} // namespace yave