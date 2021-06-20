//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

namespace yave {

#if defined(_WIN32) || defined(_WIN64)
#  define YAVE_OS_WINDOWS
#endif

#if defined(__linux)
#  define YAVE_OS_LINUX
#endif

#if defined(__unix)
#  define YAVE_OS_UNIX
#endif

#if defined(__posix)
#  define YAVE_OS_POSIX
#endif

#if defined(__APPLE__)
#  define YAVE_OS_OSX
#endif

} // namespace yave