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
#  define YAVE_MMX
#else
  constexpr bool has_MMX    = false;
#endif

#if defined(__AES__)
  constexpr bool has_AES = true;
#  define YAVE_AES
#else
  constexpr bool has_AES    = false;
#endif

#if defined(__FMA__)
  constexpr bool has_FMA = true;
#  define YAVE_FMA
#else
  constexpr bool has_FMA    = false;
#endif

#if defined(__SSE__)
  constexpr bool has_SSE = true;
#  define YAVE_SSE
#else
  constexpr bool has_SSE    = false;
#endif

#if defined(__SSE2__)
  constexpr bool has_SSE2 = true;
#  define YAVE_SSE2
#else
  constexpr bool has_SSE2   = false;
#endif

#if defined(__SSE3__)
  constexpr bool has_SSE3 = true;
#  define YAVE_SSE3
#else
  constexpr bool has_SSE3   = false;
#endif

#if defined(__SSSE3__)
  constexpr bool has_SSSE3 = true;
#  define YAVE_SSE3
#else
  constexpr bool has_SSSE3  = false;
#endif

#if defined(__SSE4_1__)
  constexpr bool has_SSE4_1 = true;
#  define YAVE_SSE4_1
#else
  constexpr bool has_SSE4_1 = false;
#endif

#if defined(__SSE4_2__)
  constexpr bool has_SSE4_2 = true;
#  define YAVE_SSE4_2
#else
  constexpr bool has_SSE4_2 = false;
#endif

#if defined(__AVX__)
  constexpr bool has_AVX = true;
#  define YAVE_AVX
#else
  constexpr bool has_AVX    = false;
#endif

#if defined(__AVX2__)
  constexpr bool has_AVX2 = true;
#  define YAVE_AVX2
#else
  constexpr bool has_AVX2   = false;
#endif

} // namespace yave