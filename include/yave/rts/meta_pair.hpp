//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>

namespace yave {

  template <class First, class Second>
  struct meta_pair
  {
    [[nodiscard]] constexpr auto first() const
    {
      return type_c<First>;
    }
    [[nodiscard]] constexpr auto second() const
    {
      return type_c<Second>;
    }
  };

  template <class First, class Second>
  static constexpr meta_pair<First, Second> pair_c {};

  template <class F, class S>
  [[nodiscard]] constexpr auto make_pair(meta_type<F>, meta_type<S>)
  {
    return pair_c<F, S>;
  }

} // namespace yave