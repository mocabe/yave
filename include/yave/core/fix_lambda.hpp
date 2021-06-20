//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <type_traits>

namespace yave {

  /// helper class to fix lambda
  template <class F>
  struct fix_lambda : private F
  {
    template <class G>
    constexpr fix_lambda(G&& g)
      : F {std::forward<G>(g)}
    {
    }

    template <typename... Args>
    constexpr decltype(auto) operator()(Args&&... args) const
    {
      return F::operator()(*this, std::forward<Args>(args)...);
    }
  };

  template <class F>
  fix_lambda(F &&)->fix_lambda<std::decay_t<F>>;
}