//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vec/vec.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("init default", "[lib][vec]")
{
  constexpr dvec1 d1 {};
  constexpr dvec2 d2 {};
  constexpr dvec3 d3 {};
  constexpr dvec4 d4 {};

  STATIC_REQUIRE(d1[0] == 0.0);
  STATIC_REQUIRE(d2[0] == 0.0 && d2[1] == 0.0);
  STATIC_REQUIRE(d3[0] == 0.0 && d3[1] == 0.0 && d3[2] == 0.0);
  STATIC_REQUIRE(d4[0] == 0.0 && d4[1] == 0.0 && d4[2] == 0.0 && d4[3] == 0.);
}

TEST_CASE("init single", "[lib][vec]")
{
  constexpr dvec1 d1 {1.};
  constexpr dvec2 d2 {1.};
  constexpr dvec3 d3 {1.};
  constexpr dvec4 d4 {1.};

  STATIC_REQUIRE(d1[0] == 1.0);
  STATIC_REQUIRE(d2[0] == 1.0 && d2[1] == 1.0);
  STATIC_REQUIRE(d3[0] == 1.0 && d3[1] == 1.0 && d3[2] == 1.0);
  STATIC_REQUIRE(d4[0] == 1.0 && d4[1] == 1.0 && d4[2] == 1.0 && d4[3] == 1.);
}

TEST_CASE("init multi", "[lib][vec]")
{
  constexpr dvec1 d1 {1.};
  constexpr dvec2 d2 {1., 1.};
  constexpr dvec3 d3 {1., 1., 1.};
  constexpr dvec4 d4 {1., 1., 1., 1.};

  STATIC_REQUIRE(d1 == dvec1 {1.});
  STATIC_REQUIRE(d2 == dvec2 {1.});
  STATIC_REQUIRE(d3 == dvec3 {1.});
  STATIC_REQUIRE(d4 == dvec4 {1.});
}

TEST_CASE("acess", "[lib][vec]")
{
  constexpr dvec1 d1 {0.};
  constexpr dvec2 d2 {0., 1.};
  constexpr dvec3 d3 {0., 1., 2.};
  constexpr dvec4 d4 {0., 1., 2., 3.};

  STATIC_REQUIRE(d1[0] == 0.);

  STATIC_REQUIRE(d2[0] == 0.);
  STATIC_REQUIRE(d2[1] == 1.);

  STATIC_REQUIRE(d3[0] == 0.);
  STATIC_REQUIRE(d3[1] == 1.);
  STATIC_REQUIRE(d3[2] == 2.);

  STATIC_REQUIRE(d4[0] == 0.);
  STATIC_REQUIRE(d4[1] == 1.);
  STATIC_REQUIRE(d4[2] == 2.);
  STATIC_REQUIRE(d4[3] == 3.);

  STATIC_REQUIRE(d1.x() == 0.);

  STATIC_REQUIRE(d2.x() == 0.);
  STATIC_REQUIRE(d2.y() == 1.);

  STATIC_REQUIRE(d3.x() == 0.);
  STATIC_REQUIRE(d3.y() == 1.);
  STATIC_REQUIRE(d3.z() == 2.);

  STATIC_REQUIRE(d4.x() == 0.);
  STATIC_REQUIRE(d4.y() == 1.);
  STATIC_REQUIRE(d4.z() == 2.);
  STATIC_REQUIRE(d4.w() == 3.);
}

TEST_CASE("ops", "[lib][vec]")
{
  constexpr dvec1 d1 {42};
  constexpr dvec2 d2 {42};
  constexpr dvec3 d3 {42};
  constexpr dvec4 d4 {42};

  STATIC_REQUIRE((((d1 + d1) / d1 - (d1 / d1)) * d1 + (d1 - d1)) == d1);
  STATIC_REQUIRE((((d2 + d2) / d2 - (d2 / d2)) * d2 + (d2 - d2)) == d2);
  STATIC_REQUIRE((((d3 + d3) / d3 - (d3 / d3)) * d3 + (d3 - d3)) == d3);
  STATIC_REQUIRE((((d4 + d4) / d4 - (d4 / d4)) * d4 + (d4 - d4)) == d4);
}