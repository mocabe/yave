//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/type_gen.hpp>
#include <yave/core/rts/object_util.hpp>

#include <vector>
#include <string>
#include <variant>
#include <algorithm>

namespace yave {

  namespace detail {
    /// get string represents type
    template <size_t MaxDepth>
    [[nodiscard]] std::string
      to_string_impl(const object_ptr<const Type>& type, size_t depth)
    {
      if (depth > MaxDepth)
        return "[...]";

      if (is_value_type(type)) {
        return "Value[" + to_string(get<value_type>(*type)) + "]";
      }

      if (is_arrow_type(type)) {
        return "(" + //
               to_string_impl<MaxDepth>(
                 get<arrow_type>(*type).captured, depth + 1) + //
               " -> " +                                        //
               to_string_impl<MaxDepth>(
                 get<arrow_type>(*type).returns, depth + 1) + //
               ")";                                           //
      }

      if (is_var_type(type)) {
        return "Var[" +                                     //
               to_string(*get_if<var_type>(type.value())) + //
               "]";                                         //
      }

      unreachable();
    }
  } // namespace detail

  /// convert type to string
  template <size_t MaxDepth = 48>
  [[nodiscard]] std::string to_string(const object_ptr<const Type>& type)
  {
    return detail::to_string_impl<MaxDepth>(type, 1);
  }

} // namespace yave