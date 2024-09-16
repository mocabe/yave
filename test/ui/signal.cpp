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

  SECTION("lock_with")
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
      t.lock_with([&] { test = true; });
      REQUIRE(test);

      test = false;
      p    = nullptr;
      t.lock_with([&] { test = true; });
      REQUIRE(!test);
    }

    SECTION("")
    {
      bool test = false;
      lock_with([&] { test = true; }, t);
      REQUIRE(test);

      test = false;
      p    = nullptr;
      t.lock_with([&] { test = true; });
      REQUIRE(!test);
    }

    SECTION("")
    {
      auto q = make_unique<foo>();
      auto s = q->get_tracker();
      p      = nullptr;

      bool test = false;
      lock_with([&] { test = true; }, t, s);
      REQUIRE(!test);
      lock_with([&] { test = true; }, s, t);
      REQUIRE(!test);
    }
  }

  SECTION("weak_ref")
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

    weak_ref ref(*p);
    REQUIRE(!ref.expired());
    REQUIRE(ref.get());

    SECTION("apply")
    {
      bool test = false;
      ref.apply([&](auto&) { test = true; });
      REQUIRE(test);

      p = nullptr;
      ref.apply([&](auto) { test = false; });
      REQUIRE(test);
      REQUIRE(ref.expired());
      REQUIRE(!ref.get());
    }

    SECTION("copy")
    {
      auto ref2 = ref;
      REQUIRE(!ref.expired());
      REQUIRE(!ref2.expired());
      REQUIRE(ref.get());
      REQUIRE(ref2.get());

      p = nullptr;
      REQUIRE(ref.expired());
      REQUIRE(ref2.expired());
      REQUIRE(!ref.get());
      REQUIRE(!ref2.get());
    }

    SECTION("move")
    {
      auto ref2 = std::move(ref);
      REQUIRE(ref.expired());
      REQUIRE(!ref.get());
      REQUIRE(!ref2.expired());
      REQUIRE(ref2.get());
    }

    SECTION("monadic")
    {
      bool test = false;

      ref //
        .and_then([&](auto& q) {
          test = true;
          REQUIRE(p.get() == &q);
        })
        .or_else([&]() { test = false; });

      REQUIRE(test);

      ref //
        .and_then([&](auto&) { p = nullptr; })
        .or_else([&]() { test = false; })
        .and_then([&](auto&) { test = true; });

      REQUIRE(!test);
    }
  }
}