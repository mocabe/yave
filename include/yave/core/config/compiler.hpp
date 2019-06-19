//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

namespace yave {

#if defined(__GNUC__)
#  define YAVE_COMPILER_GCC
#endif

#if defined(__clang__)
#  define YAVE_COMPILER_CLANG
#  undef YAVE_COMPILER_GCC
#endif

#if defined(_MSC_VER)
#  define YAVE_COMPILER_MSVC
#endif

#if defined(__INTEL_COMPILER)
#  define YAVE_COMPILER_ICC
#endif

} // namespace yave