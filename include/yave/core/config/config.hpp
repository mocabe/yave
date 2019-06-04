//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config/debug.hpp>
#include <yave/core/config/intrin.hpp>
#include <yave/core/config/likely.hpp>
#include <yave/core/config/unreachable.hpp>
#include <yave/core/config/type_traits.hpp>
#include <yave/core/config/os.hpp>
#include <yave/core/config/uuid.hpp>

#include <cstdint>
#include <cstddef>
#include <climits>
#include <limits>

namespace yave {

  using uint8_t  = std::uint8_t;
  using uint16_t = std::uint16_t;
  using uint32_t = std::uint32_t;
  using uint64_t = std::uint64_t;

  using int8_t  = std::int8_t;
  using int16_t = std::int16_t;
  using int32_t = std::int32_t;
  using int64_t = std::int64_t;

  static_assert(CHAR_BIT == 8);
  static_assert(sizeof(char) == sizeof(int8_t));
  static_assert(sizeof(unsigned char) == sizeof(uint8_t));

  static_assert(sizeof(float) == 4);
  static_assert(sizeof(double) == 8);
  static_assert(std::numeric_limits<float>::is_iec559);
  static_assert(std::numeric_limits<double>::is_iec559);

  using nullptr_t = std::nullptr_t;

#if defined(__cpp_char8_t)
  // use c++20 char8_t
#else
  enum char8_t : unsigned char
  {
  };
#endif

} // namespace yave