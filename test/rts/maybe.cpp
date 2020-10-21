//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/rts/maybe.hpp>
#include <yave/rts/eval.hpp>
#include <yave/rts/to_string.hpp>
#include <catch2/catch.hpp>
#include <iostream>

using namespace yave;

namespace yave {
  using Int    = yave::Box<int>;
  using Double = yave::Box<double>;
  using Float  = yave::Box<float>;
  using Bool   = yave::Box<bool>;
} // namespace yave

YAVE_DECL_TYPE(Int, "7d27665a-c56a-40d1-8e2e-844cb48de9e9");
YAVE_DECL_TYPE(Double, "9cc69b38-8766-44f1-93e9-337cfb3d3bc5");
YAVE_DECL_TYPE(Float, "51b6aa8c-b54a-417f-bfea-5bbff6ef00c2");
YAVE_DECL_TYPE(Bool, "7ba340e7-8c41-41bc-a1f9-bea2a2db077d");

TEST_CASE("Maybe<Int>")
{
  SECTION("make_object")
  {
    auto i = make_object<Maybe<Int>>();
    REQUIRE(i);
    REQUIRE(i->is_nothing());
    REQUIRE(!i->has_value());
    REQUIRE_THROWS(i->value());
  }

  SECTION("make_object")
  {
    auto i = make_object<Maybe<Int>>(make_object<Int>(42));
    REQUIRE(i);
    REQUIRE(!i->is_nothing());
    REQUIRE(i->has_value());
    REQUIRE(i->value());
    // REQUIRE(*i->value() == 42); should fail
    REQUIRE(*eval(i->value()) == 42);
  }

  SECTION("make_maybe")
  {
    auto i = make_maybe<Int>();
    REQUIRE(i);
    REQUIRE(i->is_nothing());
    REQUIRE(!i->has_value());
    REQUIRE_THROWS(i->value());
  }

  SECTION("make_maybe")
  {
    auto i = make_maybe<Int>(make_object<Int>(42));
    REQUIRE(i);
    REQUIRE(!i->is_nothing());
    REQUIRE(i->has_value());
    REQUIRE(i->value());
    // REQUIRE(*i->value() == 42); should fail
    REQUIRE(*eval(i->value()) == 42);
  }

  SECTION("misc")
  {
    std::cout << to_string(object_type<Maybe<Maybe<Int>>>()) << std::endl;
  }
}