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
    inline auto to_string_impl(const object_ptr<const Type>& type)
      -> std::string
    {
      if (is_arrow_type(type)) {
        auto ap1 = is_tap_type_if(type);
        auto ap2 = is_tap_type_if(ap1->t1);
        return "(" +                     //
               to_string_impl(ap2->t2) + //
               " -> " +                  //
               to_string_impl(ap1->t2) + //
               ")";                      //
      }

      if (is_list_type(type)) {
        auto ap = is_tap_type_if(type);
        return "[" +                    //
               to_string_impl(ap->t2) + //
               "]";                     //
      }

      if (auto con = is_tcon_type_if(type)) {
        return to_string(*con);
      }

      if (auto ap = is_tap_type_if(type)) {
        return "(" +                    //
               to_string_impl(ap->t1) + //
               " " +                    //
               to_string_impl(ap->t2) + //
               ")";                     //
      }

      if (auto var = is_tvar_type_if(type)) {
        return "var(" + to_string(*var) + ")";
      }

      unreachable();
    }
  } // namespace detail

  /// convert type to string
  [[nodiscard]] inline auto to_string(const object_ptr<const Type>& type)
    -> std::string
  {
    return detail::to_string_impl(type);
  }

} // namespace yave