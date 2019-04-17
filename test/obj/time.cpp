//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/time.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("time")
{
  SECTION("make_object")
  {
    [[maybe_unused]] auto v0 = make_object<Time::Time>();
    [[maybe_unused]] auto v1 = make_object<Time::Time>(0);
    [[maybe_unused]] auto v2 = make_object<Time::Time>(time::zero());
  }
  SECTION("access")
  {
    [[maybe_unused]] auto v = make_object<Time::Time>()->int_value();
  }
}