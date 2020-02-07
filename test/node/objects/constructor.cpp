//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/module/std/core/decl/primitive.hpp>
#include <yave/module/std/core/def/primitive.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/rts/rts.hpp>

using namespace yave;

TEST_CASE("Constructor")
{
  SECTION("make_object")
  {
    auto v = make_data_type_holder<Int>();
    REQUIRE(has_type<Int>(v->data()));
  }

  SECTION("eval")
  {
    auto v                      = make_data_type_holder<Int>();
    *value_cast<Int>(v->data()) = 42;
    auto r                      = eval(v << make_object<FrameDemand>());
    REQUIRE(*value_cast<Int>(r) == 42);
  }
}