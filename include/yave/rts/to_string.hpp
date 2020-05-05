//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/type_gen.hpp>
#include <yave/rts/object_util.hpp>
#include <yave/rts/dynamic_typing.hpp>

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

      if (is_arrow_type(type)) {
        auto ap1 = is_tap_type_if(type);
        auto ap2 = is_tap_type_if(ap1->t1);
        return "(" +                                          //
               to_string_impl<MaxDepth>(ap1->t2, depth + 1) + //
               " -> " +                                       //
               to_string_impl<MaxDepth>(ap2->t2, depth + 1) + //
               ")";                                           //
      }

      if (is_list_type(type)) {
        auto ap = is_tap_type_if(type);
        return "[" +                                         //
               to_string_impl<MaxDepth>(ap->t2, depth + 1) + //
               "]";                                          //
      }

      if (auto con = is_tcon_type_if(type)) {
        return to_string(*con);
      }

      if (auto ap = is_tap_type_if(type)) {
        return "(" +                                         //
               to_string_impl<MaxDepth>(ap->t1, depth + 1) + //
               " " +                                         //
               to_string_impl<MaxDepth>(ap->t2, depth + 1) + //
               ")";                                          //
      }

      if (auto var = is_tvar_type_if(type)) {
        return "var(" + to_string(*var) + ")";
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