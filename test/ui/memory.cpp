//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/memory.hpp>
#include <catch2/catch.hpp>
#include <iostream>

using namespace yave::ui;

TEST_CASE("unique")
{
  SECTION("null")
  {
    auto p = unique<int>(nullptr);
    REQUIRE(!p);
    REQUIRE(!p.get());

    auto q = unique<const int>(std::move(p));
    REQUIRE(!p);
    REQUIRE(!q);

    auto r = std::move(q);
    REQUIRE(!p);
    REQUIRE(!q);
    REQUIRE(!r);
  }

  SECTION("ptr")
  {
    auto p = make_unique<int>(42);
    REQUIRE(p);
    REQUIRE(p.get());
    REQUIRE(*p == 42);
    REQUIRE(*p.get() == 42);

    auto q = std::move(p);
    REQUIRE(!p);
    REQUIRE(q);
    REQUIRE(*q == 42);

    *q = 24;
    REQUIRE(*q == 24);

    auto r = unique<const int>(std::move(q));
    REQUIRE(!q);
    REQUIRE(r);
    REQUIRE(*r == 24);
  }

  SECTION("assign")
  {
    auto p = unique<int>();
    auto q = make_unique<int>(42);

    p = std::move(q);
    REQUIRE(p);
    REQUIRE(!q);
    REQUIRE(*p == 42);
  }

  SECTION("share")
  {
    auto p = make_unique<int>(42);
    auto q = shared(std::move(p));

    REQUIRE(!p);
    REQUIRE(q);
    REQUIRE(*q == 42);
  }

  SECTION("cast static")
  {
    auto p = make_unique<int>(42);
    auto q = std::move(p).cast_static<int>();

    REQUIRE(!p);
    REQUIRE(q);

    REQUIRE(!std::move(p).cast_static<int>());
    REQUIRE(std::move(q).cast_static<int>());
  }

  SECTION("cast dynamic")
  {
    struct Base
    {
      virtual ~Base() = default;
    };

    struct Derived : Base
    {
    };

    auto p = make_unique<Derived>();
    auto q = std::move(p).cast_dynamic<Base>();

    REQUIRE(!p);
    REQUIRE(q);
  }
}

TEST_CASE("shared")
{
  SECTION("null")
  {
    auto p = shared<int>(nullptr);
    REQUIRE(!p);
    REQUIRE(!p.get());

    auto q = shared<const int>(std::move(p));
    REQUIRE(!p);
    REQUIRE(!q);

    auto r = q;
    REQUIRE(!p);
    REQUIRE(!q);
    REQUIRE(!r);
  }

  SECTION("ptr")
  {
    auto p = make_shared<int>(42);
    REQUIRE(p);
    REQUIRE(p.get());
    REQUIRE(*p == 42);
    REQUIRE(*p.get() == 42);

    auto q = p;
    REQUIRE(p);
    REQUIRE(q);
    REQUIRE(*p == 42);
    REQUIRE(*q == 42);

    *q = 24;
    REQUIRE(*p == 24);
    REQUIRE(*q == 24);

    auto r = shared<const int>(std::move(q));
    REQUIRE(p);
    REQUIRE(!q);
    REQUIRE(r);
    REQUIRE(*p == 24);
    REQUIRE(*r == 24);
  }

  SECTION("assign")
  {
    auto p = shared<int>();
    auto q = make_shared<int>(42);

    p = q;
    REQUIRE(p);
    REQUIRE(q);
    REQUIRE(*p == 42);
    REQUIRE(*q == 42);

    q = std::move(p);
    REQUIRE(!p);
    REQUIRE(q);
  }

  SECTION("cast static")
  {
    auto p = make_shared<int>(42);
    auto q = std::move(p).cast_static<int>();

    REQUIRE(!p);
    REQUIRE(q);

    REQUIRE(!std::move(p).cast_static<int>());
    REQUIRE(std::move(q).cast_static<int>());
  }

  SECTION("cast dynamic")
  {
    struct Base
    {
      virtual ~Base() = default;
    };

    struct Derived : Base
    {
    };

    auto p = make_shared<Derived>();
    auto q = p.cast_dynamic<Base>();

    REQUIRE(p);
    REQUIRE(q);

    q = std::move(p).cast_dynamic<Base>();
    REQUIRE(!p);
    REQUIRE(q);
  }
}