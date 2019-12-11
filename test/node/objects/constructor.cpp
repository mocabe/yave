//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/decl/constructor.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/backend/default/common/primitive_constructor.hpp>
#include <yave/rts/eval.hpp>

using namespace yave;

TEST_CASE("Constructor")
{
  using IntConstructor = backends::default_common::PrimitiveConstructor<Int>;

  auto i = make_object<PrimitiveContainer>(42);

  SECTION("make_object")
  {
    auto v0 = make_object<IntConstructor>();
    auto v1 = make_object<IntConstructor>(i);
  }

  SECTION("eval")
  {
    auto v = make_object<IntConstructor>(i);
    auto r = eval(v << make_object<FrameDemand>());
    REQUIRE(*r == 42);
  }
}