//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/function.hpp>

namespace yave {

  namespace detail {
    struct id_X;
  }

  // id = \x.x
  struct Id : Function<Id, detail::id_X, detail::id_X>
  {
    auto code() const -> return_type
    {
      return arg<0>();
    }
  };

}