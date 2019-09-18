//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/vec/vec.hpp>
#include <catch2/catch.hpp>

using namespace yave;

#define TestInitObject(TYPE)                       \
  do {                                             \
    [[maybe_unused]] auto i = make_object<TYPE>(); \
    REQUIRE(i);                                    \
  } while (0)

TEST_CASE("init", "[obj][vec]")
{
  TestInitObject(I8Vec1);
  TestInitObject(I16Vec1);
  TestInitObject(I32Vec1);
  TestInitObject(I64Vec1);

  TestInitObject(I8Vec2);
  TestInitObject(I16Vec2);
  TestInitObject(I32Vec2);
  TestInitObject(I64Vec2);

  TestInitObject(I8Vec3);
  TestInitObject(I16Vec3);
  TestInitObject(I32Vec3);
  TestInitObject(I64Vec3);

  TestInitObject(I8Vec4);
  TestInitObject(I16Vec4);
  TestInitObject(I32Vec4);
  TestInitObject(I64Vec4);

  TestInitObject(IVec1);
  TestInitObject(IVec2);
  TestInitObject(IVec3);
  TestInitObject(IVec4);

  TestInitObject(U8Vec1);
  TestInitObject(U16Vec1);
  TestInitObject(U32Vec1);
  TestInitObject(U64Vec1);

  TestInitObject(U8Vec2);
  TestInitObject(U16Vec2);
  TestInitObject(U32Vec2);
  TestInitObject(U64Vec2);

  TestInitObject(U8Vec3);
  TestInitObject(U16Vec3);
  TestInitObject(U32Vec3);
  TestInitObject(U64Vec3);

  TestInitObject(U8Vec4);
  TestInitObject(U16Vec4);
  TestInitObject(U32Vec4);
  TestInitObject(U64Vec4);

  TestInitObject(UVec1);
  TestInitObject(UVec2);
  TestInitObject(UVec3);
  TestInitObject(UVec4);

  TestInitObject(FVec1);
  TestInitObject(FVec2);
  TestInitObject(FVec3);
  TestInitObject(FVec4);

  TestInitObject(DVec1);
  TestInitObject(DVec2);
  TestInitObject(DVec3);
  TestInitObject(DVec4);
}