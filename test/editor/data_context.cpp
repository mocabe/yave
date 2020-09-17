//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/data_context.hpp>
#include <yave/editor/data_command.hpp>
#include <catch2/catch.hpp>

#include <iostream>

using namespace yave;
using namespace yave::editor;

TEST_CASE("unique_context_data")
{
  unique_context_data data(42);

  REQUIRE(!data.empty());
  REQUIRE(data.type() == typeid(int));
  REQUIRE(*static_cast<int*>(data.data()) == 42);

  unique_context_data data2 = std::move(data);
  REQUIRE(data.empty());
  REQUIRE(data2.type() == typeid(int));
  REQUIRE(*static_cast<int*>(data2.data()) == 42);
}

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
    auto lck = ctx.lock();
    lck.add_data(42);

    REQUIRE_NOTHROW(lck.get_data<int>());
    REQUIRE_NOTHROW(lck.add_data(3.14));
    REQUIRE_THROWS(lck.add_data(24));
    REQUIRE_THROWS(lck.add_data(4.13));
    REQUIRE(lck.get_data<int>() == 42);

    lck.remove_data<int>();
    REQUIRE_THROWS(lck.get_data<int>());
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
    int i = 0;
    {
      data_context ctx;
      ctx.cmd(make_data_command([&i](auto&) { ++i; }, [](auto&) {}));
      ctx.cmd(make_data_command([&i](auto&) { ++i; }, [](auto&) {}));
      {
        auto lck = ctx.lock();
        auto tmp = i;
        ctx.cmd(make_data_command([&i](auto&) { ++i; }, [](auto&) {}));
        ctx.cmd(make_data_command([&i](auto&) { ++i; }, [](auto&) {}));
        REQUIRE(i == tmp);
      }
    }
    REQUIRE(i == 4);
  }
}