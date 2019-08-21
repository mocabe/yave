//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/obj/frame/frame.hpp>

using namespace yave;

TEST_CASE("frame")
{
  SECTION("make_object")
  {
    [[maybe_unused]] auto v0 = make_object<const Frame>();
    [[maybe_unused]] auto v1 = make_object<const Frame>(0);
    [[maybe_unused]] auto v2 = make_object<const Frame>(frame());
    [[maybe_unused]] auto v3 = make_object<const Frame>(time::zero());
  }
  SECTION("access")
  {
    [[maybe_unused]] auto v = make_object<const Frame>()->time;
  }
}