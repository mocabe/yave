//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/keyframe/keyframe.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("Keyframe")
{
  auto zero = time::zero();

  SECTION("int")
  {
    auto kf = make_object<KeyframeDataInt>();

    SECTION("init")
    {
      REQUIRE(kf->empty());
      REQUIRE(kf->size() == 0);
      REQUIRE(kf->find_range(zero) == std::pair {time::min(), time::max()});
      REQUIRE(kf->find_value(zero) == 0);
    }

    SECTION("insert")
    {
      kf->insert(zero, 42);
      REQUIRE(kf->find_value(zero) == 42);
    }
  }

  SECTION("float")
  {
    auto kf = make_object<KeyframeDataFloat>();

    SECTION("init")
    {
      REQUIRE(kf->find_value(zero).value == 0.f);
      REQUIRE(kf->find_value(zero).cp_left.time == time::zero());
      REQUIRE(kf->find_value(zero).cp_right.time == time::zero());
      REQUIRE(kf->find_value(zero).cp_right.value == 0);
      REQUIRE(kf->find_value(zero).cp_left.value == 0);
    }
  }

  SECTION("bool")
  {
    auto kf = make_object<KeyframeDataBool>();

    SECTION("init")
    {
      REQUIRE(kf->find_value(zero) == false);
    }
  }
}