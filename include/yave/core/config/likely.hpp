//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

// likely
#if !defined(likely)
#  if defined(__GNUC__)
#    define likely(expr) __builtin_expect(!!(expr), 1)
#  else
#    define likely(expr) expr
#  endif
#endif

// unlikely
#if !defined(unlikely)
#  if defined(__GNUC__)
#    define unlikely(expr) __builtin_expect(!!(expr), 0)
#  else
#    define unlikely(expr) expr
#  endif
#endif