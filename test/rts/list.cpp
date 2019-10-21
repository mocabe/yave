//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/rts/list.hpp>
#include <catch2/catch.hpp>

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

TEST_CASE("List<Int>")
{
  SECTION("make_object")
  {
    auto l = make_object<List<Int>>();
    REQUIRE(l);
    REQUIRE(l->is_nil());
    REQUIRE_THROWS(l->head());
    REQUIRE_THROWS(l->tail());
  }

  SECTION("make_object")
  {
    auto i = make_object<Int>();
    auto l = make_object<List<Int>>(i);
    REQUIRE(l);
    REQUIRE(!l->is_nil());
    REQUIRE(l->head());
    REQUIRE(l->tail());
    REQUIRE(eval(l->head()) == i);
    REQUIRE(eval(l->tail())->is_nil());
  }

  SECTION("make_object")
  {
    auto i = make_object<const Int>();
    auto l = make_object<List<Int>>(i);
    REQUIRE(l);
    REQUIRE(!l->is_nil());
    REQUIRE(l->head());
    REQUIRE(l->tail());
    REQUIRE(eval(l->head()) == i);
    REQUIRE(eval(l->tail())->is_nil());
  }

  SECTION("make_object")
  {
    auto l = make_object<List<Int>>(make_object<Int>(42));
    REQUIRE(l);
    REQUIRE(!l->is_nil());
    REQUIRE(l->head());
    REQUIRE(l->tail());
    REQUIRE(*eval(l->head()) == 42);
    REQUIRE(eval(l->tail())->is_nil());
  }

  SECTION("make_object")
  {
    auto l = make_object<List<Int>>(new Int(42));
    REQUIRE(l);
    REQUIRE(!l->is_nil());
    REQUIRE(l->head());
    REQUIRE(l->tail());
    REQUIRE(*eval(l->head()) == 42);
    REQUIRE(eval(l->tail())->is_nil());
  }

  SECTION("make_object")
  {
    auto i = make_object<Int>(42);
    auto l = make_object<List<Int>>(i, make_object<List<Int>>(i));
    REQUIRE(l);
    REQUIRE(!l->is_nil());
    REQUIRE(l->head() == i);
    REQUIRE_NOTHROW(l->tail());
    REQUIRE(!eval(l->tail())->is_nil());
  }

  SECTION("make_list")
  {
    auto l = make_list<Int>();
    REQUIRE(l);
    REQUIRE(l->is_nil());
    REQUIRE_THROWS(l->head());
    REQUIRE_THROWS(l->tail());
  }

  SECTION("make_list")
  {
    auto i = make_object<Int>(42);
    auto j = make_object<const Int>(24);
    auto l = make_list<Int>(i, j);
    REQUIRE(l);
    REQUIRE(*eval(l->head()) == 42);
    REQUIRE(*eval(eval(l->tail())->head()) == 24);
  }

  SECTION("make_list")
  {
    auto l = make_list<Int>(
      make_object<Int>(42), new Int(42), make_object<const Int>(42));
    REQUIRE(l);

    while (!l->is_nil()) {
      REQUIRE(*eval(l->head()) == 42);
      l = eval(l->tail());
    }
  }
}