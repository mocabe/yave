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


YAVE_DECL_TYPE(Int, "7d27665a-c56a-40d1-8e2e-844cb48de9e9");
YAVE_DECL_TYPE(Double, "9cc69b38-8766-44f1-93e9-337cfb3d3bc5");

TEST_CASE("pointer construct", "[rts][object_ptr]")
{
  SECTION("pointer")
  {
    object_ptr<Int> i = nullptr;
    object_ptr<Int> j {};
    REQUIRE(i == nullptr);
    REQUIRE(j == nullptr);
    REQUIRE(nullptr == i);
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
    auto i         = make_object<Int>(42);
    object_ptr<> j = i;
    auto k         = object_ptr<>(i);
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

TEST_CASE("operator bool", "[rts][object_ptr]")
{
  object_ptr<Int> i {};
  REQUIRE(!i);
  i = make_object<Int>(42);
  REQUIRE(i);
  i = nullptr;
  REQUIRE(!i);
}

TEST_CASE("value_cast", "[rts][object_ptr]")
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

TEST_CASE("operators", "[rts][object_ptr]")
{
  object_ptr<Double> lhs;
  object_ptr<Int> rhs;

  REQUIRE_NOTHROW(lhs == rhs);
  REQUIRE_NOTHROW(lhs != rhs);
  REQUIRE_NOTHROW(lhs == nullptr);
  REQUIRE_NOTHROW(nullptr == rhs);
  REQUIRE_NOTHROW(lhs != nullptr);
  REQUIRE_NOTHROW(nullptr != rhs);
  REQUIRE_NOTHROW(lhs < rhs);
  REQUIRE_NOTHROW(lhs <= rhs);
  REQUIRE_NOTHROW(lhs > rhs);
  REQUIRE_NOTHROW(lhs >= rhs);
  REQUIRE_NOTHROW(nullptr < rhs);
  REQUIRE_NOTHROW(nullptr <= rhs);
  REQUIRE_NOTHROW(nullptr > rhs);
  REQUIRE_NOTHROW(nullptr >= rhs);
  REQUIRE_NOTHROW(lhs < nullptr);
  REQUIRE_NOTHROW(lhs <= nullptr);
  REQUIRE_NOTHROW(lhs > nullptr);
  REQUIRE_NOTHROW(lhs >= nullptr);

  REQUIRE(lhs == lhs);
  REQUIRE(lhs <= lhs);
  REQUIRE(lhs >= lhs);
  REQUIRE(!(rhs != rhs));
  REQUIRE(!(rhs < rhs));
  REQUIRE(!(lhs > rhs));

  if (lhs < rhs) {
    REQUIRE(!(lhs >= rhs));
  } else {
    REQUIRE(lhs >= rhs);
  }

  if (lhs > rhs) {
    REQUIRE(!(lhs <= rhs));
  } else {
    REQUIRE(lhs <= rhs);
  }
}

TEST_CASE("custom allocator", "[rts][object_ptr]")
{
  std::pmr::synchronized_pool_resource res;

  auto i = make_object<Int>((std::pmr::memory_resource*)&res, 42);

  REQUIRE(i);
  REQUIRE(*i == 42);

  auto i2 = i.clone();
  *i2     = 24;
  REQUIRE(i2);
  REQUIRE(*i2 == 24);
  REQUIRE(*i == 42);
}

struct foo
{
  foo()
  {
    throw 42;
  }
  foo(const foo&) = default;
};

using Foo = Box<foo>;

YAVE_DECL_TYPE(Foo, "53190d38-71e8-4eae-9d29-5aea0da1d225");

TEST_CASE("constructor throw", "[rts][object]")
{
  try {
    auto f = make_object<Foo>();
  } catch (int i) {
    REQUIRE(i == 42);
  }
}

TEST_CASE("atomic")
{
  auto i  = make_object<Int>(42);
  auto i2 = make_object<Int>(24);

  i2.atomic_store(i, std::memory_order_relaxed);
  REQUIRE(*i == 42);
  REQUIRE(*i2 == 42);

  i2.atomic_store(make_object<Int>(24), std::memory_order_relaxed);
  REQUIRE(*i == 42);
  REQUIRE(*i2 == 24);

  REQUIRE(*make_object<Int>(42).atomic_load(std::memory_order_relaxed) == 42);
}