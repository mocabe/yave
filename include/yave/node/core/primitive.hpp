//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_info.hpp>
#include <yave/node/core/bind_info.hpp>
#include <yave/support/overloaded.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/obj/keyframe/keyframe.hpp>

#include <mutex>

namespace yave {

  /// make primitive
  template <class T, class... Arg>
  [[nodiscard]] auto make_primitive(Arg&&... args) -> primitive_t
  {
    return primitive_t(T {std::forward<Arg>(args)...});
  }

  /// get primitive node name
  [[nodiscard]] constexpr auto get_primitive_name(const primitive_t& v) -> const
    char*
  {
    return visit(
      overloaded {([](auto t) constexpr {
        return primitive_type_traits<decltype(t)>::name;
      })},
      v);
  }

  /// get primitive name list
  [[nodiscard]] auto get_primitive_name_list() -> std::vector<std::string>;

  /// get primitive node info
  [[nodiscard]] auto get_primitive_node_info(const primitive_t& v) -> node_info;

  /// get primitive info list
  [[nodiscard]] auto get_primitive_node_info_list() -> std::vector<node_info>;

  /// get primitive bind info
  template <class BackendTag>
  [[nodiscard]] auto get_primitive_bind_info(const primitive_t& v) -> bind_info;

  /// get primitive bind list
  template <class BackendTag>
  [[nodiscard]] auto get_primitive_bind_info_list() -> std::vector<bind_info>;

  /// get type of primitive object
  template <class BackendTag>
  [[nodiscard]] auto get_primitive_type(const primitive_t& v)
    -> object_ptr<const Type>;

} // namespace yave