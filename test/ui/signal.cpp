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
    auto tracker = std::make_unique<trackable>();

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

    struct T2 : trackable
    {
      int* i = nullptr;

      void init(int& p, T1& t1)
      {
        i = &p;
        t1.sig.connect(slot<int>([&](auto x) { *i += x; }).track(*this));
      }
    };

    auto t1 = std::make_unique<T1>();
    auto t2 = std::make_unique<T2>();

    int i = 0;
    t2->init(i, *t1);

    t1->foo();
    REQUIRE(i == 42);

    t2 = nullptr;
    t1->foo();
    REQUIRE(i == 42);
  }
}