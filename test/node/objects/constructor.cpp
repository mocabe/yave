//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/module/std/core/decl/primitive.hpp>
#include <yave/module/std/core/def/primitive.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/rts/eval.hpp>

using namespace yave;

TEST_CASE("Constructor")
{
  using IntConstructor = modules::_std::core::PrimitiveConstructor<Int>;
  using IntDataConstructor = PrimitiveDataCtor<Int>;

  auto i = make_object<IntDataConstructor>() << make_object<Int>(42);

  SECTION("make_object")
  {
    auto v1 = make_object<IntConstructor>();
  }

  SECTION("eval")
  {
    auto v = make_object<IntConstructor>();
    auto r = eval(v << i << make_object<FrameDemand>());
    REQUIRE(*r == 42);
  }
}