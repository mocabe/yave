//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

namespace yave {

#if defined(_WIN32) || defined(_WIN64)
  static constexpr bool is_windows = true;
#  define YAVE_OS_WINDOWS
#else
  static constexpr bool is_windows = false;
#endif

#if defined(__linux)
  static constexpr bool is_linux = true;
#  define YAVE_OS_LINUX
#else
  static constexpr bool is_linux   = false;
#endif

#if defined(__unix)
  static constexpr bool is_unix = true;
#  define YAVE_OS_UNIX
#else
  static constexpr bool is_unix    = false;
#endif

#if defined(__posix)
  static constexpr bool is_posix = true;
#  define YAVE_OS_POSIX
#else
  static constexpr bool is_posix   = false;
#endif

#if defined(__APPLE__)
  static constexpr bool is_osx = true;
#  define YAVE_OS_OSX
#else
  static constexpr bool is_osx     = false;
#endif

} // namespace yave