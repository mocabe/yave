//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/objects/time.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("time")
{
  SECTION("make_object")
  {
    [[maybe_unused]] auto v0 = make_object<const Time>();
    [[maybe_unused]] auto v1 = make_object<const Time>(0);
    [[maybe_unused]] auto v2 = make_object<const Time>(time::zero());
  }
  SECTION("access")
  {
    [[maybe_unused]] auto v = make_object<const Time>()->int_value();
  }
}