//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/list.hpp>
#include <yave/signal/function.hpp>

namespace yave {

  /// signal list speficier
  template <class T>
  using slist = list<signal<T>>;

  /// signal list object
  template <class T>
  using SList = List<signal<T>>;

}