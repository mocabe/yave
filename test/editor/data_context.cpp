//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/data_context.hpp>
#include <yave/editor/data_command.hpp>
#include <catch2/catch.hpp>

#include <iostream>
#include <thread>

using namespace yave;
using namespace yave::editor;

TEST_CASE("data_context")
{
  SECTION("empty")
  {
    data_context ctx;
    ctx.undo();
    ctx.redo();
  }

  SECTION("null")
  {
    data_context ctx;
    ctx.cmd(nullptr);
    ctx.undo();
    ctx.redo();
  }

  SECTION("data")
  {
    data_context ctx;
    ctx.add_data(42);

    REQUIRE_NOTHROW(ctx.get_data<int>());
    REQUIRE_NOTHROW(ctx.add_data(3.14));
    REQUIRE_THROWS(ctx.add_data(24));
    REQUIRE_THROWS(ctx.add_data(4.13));

    {
      auto lck = ctx.get_data<int>();
      REQUIRE(lck.ref() == 42);
    }

    ctx.remove_data<int>();
    REQUIRE_THROWS(ctx.get_data<int>());
  }

  SECTION("cmd")
  {
    int c = 0;
    {
      data_context ctx;
      ctx.cmd(make_data_command([&c](auto&) { c += 1; }, [](auto&) {}));
      ctx.cmd(make_data_command([&c](auto&) { c *= 2; }, [](auto&) {}));
      ctx.cmd(make_data_command([&c](auto&) { c -= 3; }, [](auto&) {}));
    }
    REQUIRE(c == -1);
  }

  SECTION("undo/redo")
  {
    int c = 0;
    {
      data_context ctx;

      ctx.cmd(
        make_data_command([&c](auto&) { c += 1; }, [&c](auto&) { c -= 1; }));
      ctx.cmd(
        make_data_command([&c](auto&) { c += 2; }, [&c](auto&) { c -= 2; }));
      ctx.cmd(
        make_data_command([&c](auto&) { c += 3; }, [&c](auto&) { c -= 3; }));

      // c == 6

      ctx.undo();
      ctx.undo();
      ctx.undo();

      // c == 0

      ctx.redo();
      ctx.redo();
      ctx.undo();
      ctx.undo();

      // c == 0

      for (auto i = 0; i < 3; ++i) {
        ctx.redo();
        ctx.redo();
        ctx.undo();
      }

      ctx.redo();

      // c == 6
    }
    REQUIRE(c == 6);
  }

  SECTION("exception")
  {
    data_context ctx;
    ctx.cmd(make_data_command([](auto&) { throw 42; }, [](auto&) {}));

    try {
      ctx.undo();
    } catch (int i) {
      REQUIRE(i == 42);
    }
  }

  SECTION("data lock")
  {
    std::atomic<int> i = 0;
    {
      data_context ctx;
      ctx.add_data(42);

      ctx.cmd(make_data_command([&i](auto&) { ++i; }, [](auto&) {}));
      ctx.cmd(make_data_command([&i](auto&) { ++i; }, [](auto&) {}));

      // wait tasks
      while (i != 2)
        ;

      {
        auto lck = ctx.get_data<int>();
        auto tmp = i.load();

        ctx.cmd(make_data_command(
          [&i](auto& c) {
            c.template get_data<int>();
            ++i;
          },
          [](auto&) {}));

        ctx.cmd(make_data_command(
          [&i](auto& c) {
            c.template get_data<int>();
            ++i;
          },
          [](auto&) {}));

        // lck blocks tasks
        REQUIRE(i == tmp);
      }
    }
    REQUIRE(i == 4);
  }
}