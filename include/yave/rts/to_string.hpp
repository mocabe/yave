//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/type_gen.hpp>
#include <yave/rts/object_util.hpp>

#include <vector>
#include <string>
#include <variant>
#include <algorithm>

namespace yave {

  namespace detail {
    /// get string represents type
    template <size_t MaxDepth>
    auto to_string_impl(const object_ptr<const Type>& type, size_t depth)
      -> std::string
    {
      if (depth > MaxDepth)
        return "[...]";

      if (auto value = get_if<value_type>(type.value())) {
        return "Value[" + to_string(*value) + "]";
      }

      if (auto arrow = get_if<arrow_type>(type.value())) {
        return "(" +                                                  //
               to_string_impl<MaxDepth>(arrow->captured, depth + 1) + //
               " -> " +                                               //
               to_string_impl<MaxDepth>(arrow->returns, depth + 1) +  //
               ")";                                                   //
      }

      if (auto var = get_if<var_type>(type.value())) {
        return "Var[" + to_string(*var) + "]";
      }

      if (auto list = get_if<list_type>(type.value())) {
        return "List<" + to_string_impl<MaxDepth>(list->t, depth + 1) + ">";
      }

      unreachable();
    }
  } // namespace detail

  /// convert type to string
  template <size_t MaxDepth = 48>
  [[nodiscard]] auto to_string(const object_ptr<const Type>& type)
    -> std::string
  {
    return detail::to_string_impl<MaxDepth>(type, 1);
  }

} // namespace yave