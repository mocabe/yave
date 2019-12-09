//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/function.hpp>

namespace yave {

  namespace detail {
    struct identity_X;
  }

  // id = \x.x
  struct Identity : Function<Identity, detail::identity_X, detail::identity_X>
  {
    auto code() const -> return_type
    {
      return arg<0>();
    }
  };
}