//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vec/tvec.hpp>

namespace yave {

  // int

  using i8vec1  = tvec1<int8_t>;
  using i16vec1 = tvec1<int16_t>;
  using i32vec1 = tvec1<int32_t>;
  using i64vec1 = tvec1<int64_t>;

  using i8vec2  = tvec2<int8_t>;
  using i16vec2 = tvec2<int16_t>;
  using i32vec2 = tvec2<int32_t>;
  using i64vec2 = tvec2<int64_t>;

  using i8vec3  = tvec3<int8_t>;
  using i16vec3 = tvec3<int16_t>;
  using i32vec3 = tvec3<int32_t>;
  using i64vec3 = tvec3<int64_t>;

  using i8vec4  = tvec4<int8_t>;
  using i16vec4 = tvec4<int16_t>;
  using i32vec4 = tvec4<int32_t>;
  using i64vec4 = tvec4<int64_t>;

  // uint

  using u8vec1  = tvec1<uint8_t>;
  using u16vec1 = tvec1<uint16_t>;
  using u32vec1 = tvec1<uint32_t>;
  using u64vec1 = tvec1<uint64_t>;

  using u8vec2  = tvec2<uint8_t>;
  using u16vec2 = tvec2<uint16_t>;
  using u32vec2 = tvec2<uint32_t>;
  using u64vec2 = tvec2<uint64_t>;

  using u8vec3  = tvec3<uint8_t>;
  using u16vec3 = tvec3<uint16_t>;
  using u32vec3 = tvec3<uint32_t>;
  using u64vec3 = tvec3<uint64_t>;

  using u8vec4  = tvec4<uint8_t>;
  using u16vec4 = tvec4<uint16_t>;
  using u32vec4 = tvec4<uint32_t>;
  using u64vec4 = tvec4<uint64_t>;

  // float

  using fvec1 = tvec1<float>;
  using fvec2 = tvec2<float>;
  using fvec3 = tvec3<float>;
  using fvec4 = tvec4<float>;

  // double

  using dvec1 = tvec1<double>;
  using dvec2 = tvec2<double>;
  using dvec3 = tvec3<double>;
  using dvec4 = tvec4<double>;

} // namespace yave