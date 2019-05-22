//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_info.hpp>
#include <yave/node/core/bind_info.hpp>
#include <yave/support/overloaded.hpp>
#include <yave/core/objects/primitive.hpp>

#include <mutex>

namespace yave {

  /// make primitive
  template <class T, class... Arg>
  [[nodiscard]] primitive_t make_primitive(Arg&&... args)
  {
    return primitive_t(T {std::forward<Arg>(args)...});
  }

  /// get primitive node name
  [[nodiscard]] constexpr const char* get_primitive_name(const primitive_t& v)
  {
    return std::visit(
      overloaded {([](auto t) constexpr {
        return primitive_type_traits<decltype(t)>::name;
      })},
      v);
  }

  /// get primitive object constructor
  [[nodiscard]] object_ptr<> get_primitive_constructor(const primitive_t& v);

  /// get type of primitive object
  [[nodiscard]] object_ptr<const Type> get_primitive_type(const primitive_t& v);

  /// get primitive node info
  [[nodiscard]] node_info get_primitive_info(const primitive_t& v);

  /// get primitive bind info
  [[nodiscard]] bind_info get_primitive_bind_info(const primitive_t& v);

  /// get primitive name list
  [[nodiscard]] std::vector<std::string> get_primitive_name_list();

  /// get primitive info list
  [[nodiscard]] std::vector<node_info> get_primitive_info_list();

  /// get primitive bind list
  [[nodiscard]] std::vector<bind_info> get_primitive_bind_info_list();
} // namespace yave