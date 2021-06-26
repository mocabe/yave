//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

namespace yave {

  namespace detail {

    template <typename T1, typename T2>
    struct offset_of_impl
    {
      union U
      {
        U()
          : c {}
        {
        }
        ~U()
        {
        }
        char c[sizeof(T2)];
        T2 o;
      };
      static U u;
      static constexpr size_t get(T1 T2::*member)
      {
        size_t i = 0;
        for (; i < sizeof(T2); ++i)
          if (((void*)&(u.c[i])) == &(u.o.*member))
            break;

        // g++ bug 67371 workaround
        if (i >= sizeof(T2))
          throw;
        else
          return i;
      }
    };

    template <class T1, class T2>
    typename offset_of_impl<T1, T2>::U offset_of_impl<T1, T2>::u {};

  } // namespace detail

  /// Get offset of member at compile time.
  /// i.e. constexpr offsetof.
  template <class T1, class T2>
  [[nodiscard]] consteval size_t offset_of(T1 T2::*member)
  {
    return detail::offset_of_impl<T1, T2>::get(member);
  }

} // namespace yave
