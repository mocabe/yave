//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#if defined(_MSC_VER)
#  include <intrin.h>
#else
#  include <x86intrin.h>
#endif

namespace yave {

#if defined(__MMX__)
  constexpr bool has_MMX = true;
#else
  constexpr bool has_MMX    = false;
#endif

#if defined(__AES__)
  constexpr bool has_AES = true;
#else
  constexpr bool has_AES    = false;
#endif

#if defined(__FMA__)
  constexpr bool has_FMA = true;
#else
  constexpr bool has_FMA    = false;
#endif

#if defined(__SSE__)
  constexpr bool has_SSE = true;
#else
  constexpr bool has_SSE    = false;
#endif

#if defined(__SSE2__)
  constexpr bool has_SSE2 = true;
#else
  constexpr bool has_SSE2   = false;
#endif

#if defined(__SSE3__)
  constexpr bool has_SSE3 = true;
#else
  constexpr bool has_SSE3   = false;
#endif

#if defined(__SSSE3__)
  constexpr bool has_SSSE3 = true;
#else
  constexpr bool has_SSSE3  = false;
#endif

#if defined(__SSE4_1__)
  constexpr bool has_SSE4_1 = true;
#else
  constexpr bool has_SSE4_1 = false;
#endif

#if defined(__SSE4_2__)
  constexpr bool has_SSE4_2 = true;
#else
  constexpr bool has_SSE4_2 = false;
#endif

#if defined(__AVX__)
  constexpr bool has_AVX = true;
#else
  constexpr bool has_AVX    = false;
#endif

#if defined(__AVX2__)
  constexpr bool has_AVX2 = true;
#else
  constexpr bool has_AVX2   = false;
#endif

} // namespace yave