//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>
#include <yave/rts/object_ptr.hpp>
#include <yave/rts/box.hpp>
#include <yave/rts/value_cast.hpp>

using namespace yave;

namespace yave {
  using Int    = yave::Box<int>;
  using Double = yave::Box<double>;
} // namespace yave

YAVE_DECL_TYPE(Int);
YAVE_DECL_TYPE(Double);

TEST_CASE("pointer construct")
{
  SECTION("pointer")
  {
    object_ptr<Int> i = nullptr;
    object_ptr<Int> j {};
    REQUIRE(i == nullptr);
    REQUIRE(j == nullptr);
    REQUIRE(nullptr == i);
  }
  SECTION("pointer")
  {
    object_ptr<Int> i = new Int(42);
    REQUIRE(i);
    REQUIRE(*i == 42);
  }
  SECTION("deduction")
  {
    object_ptr i = new Int(42);
    REQUIRE(i);
    REQUIRE(*i == 42);
  }
  SECTION("deduction")
  {
    auto i = object_ptr(new Int(42));
    REQUIRE(i);
    REQUIRE(*i == 42);
  }
  SECTION("guided")
  {
    {
      // deduction guide for `object_ptr()` does not work on gcc until GCC9.
      // bug tracker URL: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81486
#if !defined(__GNUC__)
      auto p = object_ptr();
      REQUIRE(!p);
#endif
    }
    {
      auto p = object_ptr {};
      REQUIRE(!p);
    }
    {
      auto p = object_ptr(nullptr);
      REQUIRE(!p);
    }
    {
      auto p = object_ptr {nullptr};
      REQUIRE(!p);
    }
  }
  SECTION("make_object")
  {
    auto i = make_object<Int>(42);
    REQUIRE(i);
    REQUIRE(*i == 42);
  }
  SECTION("copy")
  { 
    auto i = make_object<Int>(42);
    auto j = i;
    REQUIRE(j);
    REQUIRE(*j == 42);
  }
  SECTION("move")
  {
    auto i = make_object<Int>(42);
    auto j = std::move(i);
    REQUIRE(j);
    REQUIRE(*j == 42);
  }
  SECTION("pointer comparison")
  {
    auto i = make_object<Int>(42);
    auto j = i;
    REQUIRE(i == j);
    REQUIRE(!(i != j));
  }
  SECTION("conversion")
  {
    auto i = make_object<Int>(42);
    object_ptr<> j = i;
    auto k = object_ptr<>(i);
    REQUIRE(i == j);
    REQUIRE(!(i != j));
  }
  SECTION("conversion")
  {
    // clang-format off
    static_assert(std::is_constructible_v<object_ptr<const Object>, object_ptr<Object>>);
    static_assert(std::is_constructible_v<object_ptr<Object>, object_ptr<Int>>);
    static_assert(std::is_constructible_v<object_ptr<const Object>, object_ptr<const Int>>);
    static_assert(std::is_constructible_v<object_ptr<const Object>, object_ptr<Int>>);
    static_assert(!std::is_constructible_v<object_ptr<Object>, object_ptr<const Object>>);
    static_assert(!std::is_constructible_v<object_ptr<Object>, object_ptr<const Int>>);
    static_assert(!std::is_constructible_v<object_ptr<Int>, object_ptr<Object>>);
    // clang-format on
  }
}

TEST_CASE("operator bool")
{
  SECTION("pointer")
  {
    object_ptr<Int> i {};
    REQUIRE(!i);
    i = new Int(42);
    REQUIRE(i);
    i = nullptr;
    REQUIRE(!i);
  }
}

TEST_CASE("value_cast")
{
  SECTION("value_cast")
  {
    object_ptr<> i = make_object<Int>(42);
    REQUIRE_THROWS_AS(value_cast<Double>(i), bad_value_cast);
    REQUIRE(*value_cast<Int>(i) == 42);
  }
  SECTION("value_cast_if")
  {
    object_ptr<> i = make_object<Int>(42);
    REQUIRE(!value_cast_if<Double>(i));
    REQUIRE(*value_cast_if<Int>(i) == 42);
  }
}