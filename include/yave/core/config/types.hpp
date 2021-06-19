//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <cstdint>
#include <climits>
#include <limits>

namespace yave {

  using u8  = std::uint8_t;
  using u16 = std::uint16_t;
  using u32 = std::uint32_t;
  using u64 = std::uint64_t;

  using i8  = std::int8_t;
  using i16 = std::int16_t;
  using i32 = std::int32_t;
  using i64 = std::int64_t;

  using f32 = float;
  using f64 = double;

  static_assert(CHAR_BIT == 8);
  static_assert(sizeof(char) == sizeof(i8));
  static_assert(sizeof(unsigned char) == sizeof(u8));
  static_assert(sizeof(bool) == sizeof(char));
  static_assert(alignof(bool) == alignof(char));

  static_assert(sizeof(float) == 4);
  static_assert(sizeof(double) == 8);
  static_assert(std::numeric_limits<float>::is_iec559);
  static_assert(std::numeric_limits<double>::is_iec559);

} // namespace yave