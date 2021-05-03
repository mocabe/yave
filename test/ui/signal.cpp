//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/signal.hpp>
#include <catch2/catch.hpp>
#include <iostream>

using namespace yave::ui;

TEST_CASE("signal")
{
  SECTION("basic")
  {
    signal<int> sig;
    int i = 0;
    sig.connect([&](auto x) { i = x; });
    sig(42);
    REQUIRE(i == 42);
  }

  SECTION("trackable")
  {
    struct foo : trackable, enable_tracker_from_this<foo>
    {
      auto get_tracker() const -> tracker
      {
        return tracker_from_this();
      }
    };

    auto tracker = make_unique<foo>();

    signal<int> sig;
    int i = 0;
    sig.connect(slot<int>([&](auto x) { i += x; }).track(*tracker));
    sig(42);
    tracker = nullptr;
    sig(24);
    REQUIRE(i == 42);
  }

  SECTION("trackable")
  {
    struct T1
    {
      signal<int> sig;

      void foo()
      {
        sig(42);
      }
    };

    struct T2 : trackable, enable_tracker_from_this<T2>
    {
      int* i = nullptr;

      void init(int& p, T1& t1)
      {
        i = &p;
        t1.sig.connect(slot<int>([&](auto x) { *i += x; }).track(*this));
      }

      auto get_tracker() const -> tracker
      {
        return tracker_from_this();
      }
    };

    auto t1 = make_unique<T1>();
    auto t2 = make_unique<T2>();

    int i = 0;
    t2->init(i, *t1);

    t1->foo();
    REQUIRE(i == 42);

    t2 = nullptr;
    t1->foo();
    REQUIRE(i == 42);
  }

  SECTION("try_lock")
  {
    struct foo : trackable, enable_tracker_from_this<foo>
    {
      auto get_tracker() const -> tracker
      {
        return tracker_from_this();
      }
    };

    auto p = make_unique<foo>();
    auto t = p->get_tracker();

    REQUIRE(!t.expired());

    SECTION("")
    {
      bool test = false;
      REQUIRE(t.try_lock([&] { test = true; }));
      REQUIRE(test);

      test = false;
      p    = nullptr;
      REQUIRE(!t.try_lock([&] { test = true; }));
      REQUIRE(!test);
    }

    SECTION("")
    {
      bool test = false;
      REQUIRE(try_lock([&] { test = true; }, t));
      REQUIRE(test);

      test = false;
      p    = nullptr;
      REQUIRE(!t.try_lock([&] { test = true; }));
      REQUIRE(!test);
    }

    SECTION("")
    {
      auto q = make_unique<foo>();
      auto s = q->get_tracker();
      p      = nullptr;

      bool test = false;
      REQUIRE(!try_lock([&] { test = true; }, t, s));
      REQUIRE(!test);
      REQUIRE(!try_lock([&] { test = true; }, s, t));
      REQUIRE(!test);
    }
  }
}