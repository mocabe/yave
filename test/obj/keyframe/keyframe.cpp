//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/keyframe/keyframe.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("Keyframe")
{
  SECTION("int")
  {
    auto kf = make_object<KeyframeInt>();

    SECTION("init")
    {
      REQUIRE(kf->empty());
      REQUIRE(kf->size() == 0);
      REQUIRE(kf->find_range(0) == std::pair {time::min(), time::max()});
      REQUIRE(kf->find_value(0) == 0);
    }

    SECTION("insert")
    {
      kf->insert(0, 42);
      REQUIRE(kf->find_value(0) == 42);
    }
  }

  SECTION("float")
  {
    auto kf = make_object<KeyframeFloat>();

    SECTION("init")
    {
      REQUIRE(kf->find_value(0) == 0.f);
    }
  }

  SECTION("bool")
  {
    auto kf = make_object<KeyframeBool>();

    SECTION("init")
    {
      REQUIRE(kf->find_value(0) == false);
    }
  }
}