//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/vec/vec.hpp>

namespace yave {

  // Int

  using I8Vec1  = Box<i8vec1>;
  using I16Vec1 = Box<i16vec1>;
  using I32Vec1 = Box<i32vec1>;
  using I64Vec1 = Box<i64vec1>;

  using I8Vec2  = Box<i8vec2>;
  using I16Vec2 = Box<i16vec2>;
  using I32Vec2 = Box<i32vec2>;
  using I64Vec2 = Box<i64vec2>;

  using I8Vec3  = Box<i8vec3>;
  using I16Vec3 = Box<i16vec3>;
  using I32Vec3 = Box<i32vec3>;
  using I64Vec3 = Box<i64vec3>;

  using I8Vec4  = Box<i8vec4>;
  using I16Vec4 = Box<i16vec4>;
  using I32Vec4 = Box<i32vec4>;
  using I64Vec4 = Box<i64vec4>;

  using IVec1 = I32Vec1;
  using IVec2 = I32Vec2;
  using IVec3 = I32Vec3;
  using IVec4 = I32Vec4;

  // UInt

  using U8Vec1  = Box<u8vec1>;
  using U16Vec1 = Box<u16vec1>;
  using U32Vec1 = Box<u32vec1>;
  using U64Vec1 = Box<u64vec1>;

  using U8Vec2  = Box<u8vec2>;
  using U16Vec2 = Box<u16vec2>;
  using U32Vec2 = Box<u32vec2>;
  using U64Vec2 = Box<u64vec2>;

  using U8Vec3  = Box<u8vec3>;
  using U16Vec3 = Box<u16vec3>;
  using U32Vec3 = Box<u32vec3>;
  using U64Vec3 = Box<u64vec3>;

  using U8Vec4  = Box<u8vec4>;
  using U16Vec4 = Box<u16vec4>;
  using U32Vec4 = Box<u32vec4>;
  using U64Vec4 = Box<u64vec4>;

  using UVec1 = U32Vec1;
  using UVec2 = U32Vec2;
  using UVec3 = U32Vec3;
  using UVec4 = U32Vec4;

  // Float

  using FVec1 = Box<fvec1>;
  using FVec2 = Box<fvec2>;
  using FVec3 = Box<fvec3>;
  using FVec4 = Box<fvec4>;

  // Double

  using DVec1 = Box<dvec1>;
  using DVec2 = Box<dvec2>;
  using DVec3 = Box<dvec3>;
  using DVec4 = Box<dvec4>;
}

// clang-format off

// Int

YAVE_DECL_TYPE(yave::I8Vec1,  "29ff3fda-70e6-4574-97a0-9c295c4daaab");
YAVE_DECL_TYPE(yave::I16Vec1, "259656d9-b437-41be-b5b3-32a9a0af3bab");
YAVE_DECL_TYPE(yave::I32Vec1, "424d6c4f-9331-4951-b626-e75e237325bb");
YAVE_DECL_TYPE(yave::I64Vec1, "8b421485-b9e4-4843-b28f-81e4cdb8c882");

YAVE_DECL_TYPE(yave::I8Vec2,  "f1ea08cc-1eec-4099-b5da-5e473a5ca9be");
YAVE_DECL_TYPE(yave::I16Vec2, "fbef8328-78d1-460c-aa24-f18795722f75");
YAVE_DECL_TYPE(yave::I32Vec2, "8e67e8ff-7ba4-451f-be55-ce2cb2ee60bb");
YAVE_DECL_TYPE(yave::I64Vec2, "628ac719-c5b4-4385-ac34-b458bf887d9b");

YAVE_DECL_TYPE(yave::I8Vec3,  "4203d167-fbe9-4970-ad9e-8a6ebcf51d3b");
YAVE_DECL_TYPE(yave::I16Vec3, "58cf7203-fde7-41cd-b211-c8a0de3cc565");
YAVE_DECL_TYPE(yave::I32Vec3, "ed33da0e-3f8b-46ea-abd0-31650cc2679b");
YAVE_DECL_TYPE(yave::I64Vec3, "a6d95647-52c1-4714-8c76-64c055c20de3");

YAVE_DECL_TYPE(yave::I8Vec4,  "3a12b1c4-1bc1-40ad-b8eb-2a1b7bdf8765");
YAVE_DECL_TYPE(yave::I16Vec4, "3a8aa664-d53b-4b76-ab8f-4383df766170");
YAVE_DECL_TYPE(yave::I32Vec4, "75b1e972-21e4-4ba9-81da-18275f060112");
YAVE_DECL_TYPE(yave::I64Vec4, "f051339a-35b8-46f1-8231-f81963bdde1d");

// UInt

YAVE_DECL_TYPE(yave::U8Vec1,  "4b552535-81f5-4e37-b802-fc23a693d18d");
YAVE_DECL_TYPE(yave::U16Vec1, "4ebf1a64-d62f-4306-8160-8ddeea878e5f");
YAVE_DECL_TYPE(yave::U32Vec1, "5bbf2e5d-c8ff-45fe-b8aa-94ea6553a3f2");
YAVE_DECL_TYPE(yave::U64Vec1, "ee066616-547b-4898-8b13-08367b101f82");

YAVE_DECL_TYPE(yave::U8Vec2,  "8a6960fe-f672-4002-b756-0644486f5746");
YAVE_DECL_TYPE(yave::U16Vec2, "d1ff6099-9208-462c-8609-b38a59775015");
YAVE_DECL_TYPE(yave::U32Vec2, "1ab68e5b-579f-4489-b42b-11457659c685");
YAVE_DECL_TYPE(yave::U64Vec2, "f2866df0-3445-4926-820e-fe167fabfdb0");

YAVE_DECL_TYPE(yave::U8Vec3,  "f06c2787-b95b-4b94-ba0f-6bbfc18098f1");
YAVE_DECL_TYPE(yave::U16Vec3, "7f833fcb-00cd-47b0-a405-e223bac773b1");
YAVE_DECL_TYPE(yave::U32Vec3, "44981051-491b-4cd3-a070-d49ea67f0195");
YAVE_DECL_TYPE(yave::U64Vec3, "fd19191f-b9a6-4855-840f-c6a925d6fab9");

YAVE_DECL_TYPE(yave::U8Vec4,  "2daa1768-1e73-46c9-8b22-64f202da10c6");
YAVE_DECL_TYPE(yave::U16Vec4, "a5597cc3-bf0e-499f-8ef4-16b54949ab82");
YAVE_DECL_TYPE(yave::U32Vec4, "9c5b3b9f-c34c-40ff-95b0-ae954208a821");
YAVE_DECL_TYPE(yave::U64Vec4, "b5e36180-f253-441c-a5df-79fc4154fae4");

// Float

YAVE_DECL_TYPE(yave::FVec1,   "12a07dcc-323c-4679-a3d1-9b60b267d15d");
YAVE_DECL_TYPE(yave::FVec2,   "d67a2002-9ab3-4539-950f-ea5051577e80");
YAVE_DECL_TYPE(yave::FVec3,   "109457af-8055-43b4-b666-31e5952fb1ff");
YAVE_DECL_TYPE(yave::FVec4,   "563e45dd-1ba6-4e5f-9d66-131177de4f34");

// Double

YAVE_DECL_TYPE(yave::DVec1,   "9249315c-a062-4545-9063-6ed80df95d5e");
YAVE_DECL_TYPE(yave::DVec2,   "a3b74ed6-ae4f-437a-ad58-d6ff679cc25c");
YAVE_DECL_TYPE(yave::DVec3,   "d0a25c41-e9b8-4ff8-a1dd-ca9687087213");
YAVE_DECL_TYPE(yave::DVec4,   "90e4a48f-4d22-449e-911f-ec0627aa2009");

// clang-format on