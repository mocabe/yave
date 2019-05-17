//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

// unreachable
#if !defined(unreachable)
#  if defined(__GNUC__)
#    define unreachable() \
      assert(false);      \
      __builtin_unreachable()
#  elif defined(_MSC_VER)
#    define unreachable() \
      assert(false);      \
      __assume(0)
#  else
#    define unreachable() assert(false)
#  endif
#endif
