//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/func/constructor.hpp>
#include <yave/rts/eval.hpp>
#include <catch2/catch.hpp>

using namespace yave;

using Int = Box<int>;
YAVE_DECL_TYPE(Int, Int);

TEST_CASE("Constructor")
{
  using IntConstructor = Constructor<Int>;

  SECTION("make_object")
  {
    auto v0 = make_object<IntConstructor>();
    auto v1 = make_object<IntConstructor>(42);
    auto v2 = make_object<IntConstructor>(make_object<Int>(42));
  }

  SECTION("eval")
  {
    auto v = make_object<IntConstructor>(42);
    auto r = eval(v << make_object<Frame::Frame>());
    REQUIRE(*r == 42);
  }
}