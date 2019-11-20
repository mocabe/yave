//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/node_definition.hpp>
#include <yave/support/overloaded.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/obj/keyframe/keyframe.hpp>
#include <yave/obj/filesystem/path.hpp>

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
    return visit(
      overloaded {([](auto t) constexpr {
        return primitive_type_traits<decltype(t)>::name;
      })},
      v);
  }

  /// get primitive name list
  [[nodiscard]] auto get_primitive_name_list() -> std::vector<std::string>;

  /// get primitive node info
  [[nodiscard]] auto get_primitive_node_declaration(const primitive_t& v)
    -> node_declaration;

  /// get primitive info list
  [[nodiscard]] auto get_primitive_node_declaration_list()
    -> std::vector<node_declaration>;

  /// get primitive bind info
  template <class BackendTag>
  [[nodiscard]] auto get_primitive_node_definition(const primitive_t& v)
    -> node_definition;

  /// get primitive bind list
  template <class BackendTag>
  [[nodiscard]] auto get_primitive_node_definition_list()
    -> std::vector<node_definition>;

  /// get type of primitive object
  template <class BackendTag>
  [[nodiscard]] auto get_primitive_type(const primitive_t& v)
    -> object_ptr<const Type>;

} // namespace yave