//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#if defined(YAVE_COMPILER_GCC) && __GNUC__ < 8
#  include <experimental/filesystem>
namespace std {
  namespace filesystem = experimental::filesystem;
}
#else
#  include <filesystem>
#endif