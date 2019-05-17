//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <variant>

namespace yave {

  template <class... Ts>
  struct overloaded : Ts...
  {
    using Ts::operator()...;
  };

  template <class... Ts>
  overloaded(Ts...)->overloaded<Ts...>;

} // namespace yave