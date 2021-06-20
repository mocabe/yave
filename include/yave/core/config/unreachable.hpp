//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <cassert>

// unreachable
#if !defined(unreachable)
#  if defined(__GNUC__)
#    define unreachable()        \
      do {                       \
        assert(false);           \
        __builtin_unreachable(); \
      } while (0)
#  elif defined(_MSC_VER)
#    define unreachable() \
      do {                \
        assert(false);    \
        __assume(0);      \
      } while (0)
#  else
#    define unreachable() assert(false)
#  endif
#endif
