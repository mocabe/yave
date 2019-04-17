//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/node_info.hpp>
#include <yave/node/bind_info.hpp>
#include <yave/tools/overloaded.hpp>
#include <yave/obj/primitive.hpp>

namespace yave {

  /// make primitive
  template <class T, class... Arg>
  constexpr primitive_t make_primitive(Arg&&... args)
  {
    return primitive_t(T {std::forward<Arg>(args)...});
  }

  /// get primitive node name
  constexpr const char* get_primitive_name(const primitive_t& v)
  {
    return std::visit(
      overloaded {([](auto t) constexpr {
        return primitive_type_traits<decltype(t)>::name;
      })},
      v);
  }

  /// get primitive object instance
  object_ptr<> get_primitive_instance(const primitive_t& v);

  /// get type of primitive object
  object_ptr<const Type> get_primitive_type(const primitive_t& v);

  /// get primitive node info
  NodeInfo get_primitive_info(const primitive_t& v);

  /// get primitive bind info
  BindInfo get_primitive_bind_info(const primitive_t& v);

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

  /// get primitive name list
  std::vector<std::string> get_primitive_name_list();

  /// get primitive info list
  std::vector<NodeInfo> get_primitive_info_list();

  /// get primitive bind list
  std::vector<BindInfo> get_primitive_bind_info_list();
} // namespace yave