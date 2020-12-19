//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/maybe.hpp>
#include <yave/signal/function.hpp>

namespace yave {

  /// signal maybe
  template <class T>
  using smaybe = maybe<signal<T>>;

  /// signal maybe
  template <class T>
  using SMaybe = Maybe<signal<T>>;

} 