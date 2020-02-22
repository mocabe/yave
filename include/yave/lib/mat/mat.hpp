//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/mat/tmat.hpp>

namespace yave {

  // int

  using i8mat2  = tmat2<int8_t>;
  using i16mat2 = tmat2<int16_t>;
  using i32mat2 = tmat2<int32_t>;
  using i64mat2 = tmat2<int64_t>;

  using i8mat3  = tmat3<int8_t>;
  using i16mat3 = tmat3<int16_t>;
  using i32mat3 = tmat3<int32_t>;
  using i64mat3 = tmat3<int64_t>;

  using i8mat4  = tmat4<int8_t>;
  using i16mat4 = tmat4<int16_t>;
  using i32mat4 = tmat4<int32_t>;
  using i64mat4 = tmat4<int64_t>;

  // uint

  using u8mat2  = tmat2<uint8_t>;
  using u16mat2 = tmat2<uint16_t>;
  using u32mat2 = tmat2<uint32_t>;
  using u64mat2 = tmat2<uint64_t>;

  using u8mat3  = tmat3<uint8_t>;
  using u16mat3 = tmat3<uint16_t>;
  using u32mat3 = tmat3<uint32_t>;
  using u64mat3 = tmat3<uint64_t>;

  using u8mat4  = tmat4<uint8_t>;
  using u16mat4 = tmat4<uint16_t>;
  using u32mat4 = tmat4<uint32_t>;
  using u64mat4 = tmat4<uint64_t>;

  // float

  using fmat2 = tmat2<float>;
  using fmat3 = tmat3<float>;
  using fmat4 = tmat4<float>;

  // double

  using dmat2 = tmat2<double>;
  using dmat3 = tmat3<double>;
  using dmat4 = tmat4<double>;
}