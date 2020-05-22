//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/data_context.hpp>
#include <catch2/catch.hpp>

#include <iostream>

using namespace yave;

TEST_CASE("data_context")
{
  SECTION("empty")
  {
    editor::data_context ctx;
    ctx.undo();
    ctx.redo();
  }

  SECTION("null")
  {
    editor::data_context ctx;
    ctx.exec(nullptr);
    ctx.undo();
    ctx.redo();
  }

  SECTION("exec")
  {
    int c = 0;
    {
      editor::data_context ctx;
      ctx.exec(
        editor::make_data_command([&c](auto&) { c += 1; }, [](auto&) {}));
      ctx.exec(
        editor::make_data_command([&c](auto&) { c *= 2; }, [](auto&) {}));
      ctx.exec(
        editor::make_data_command([&c](auto&) { c -= 3; }, [](auto&) {}));
    }
    REQUIRE(c == -1);
  }

  SECTION("undo/redo")
  {
    int c = 0;
    {
      editor::data_context ctx;

      ctx.exec(editor::make_data_command(
        [&c](auto&) { c += 1; }, [&c](auto&) { c -= 1; }));
      ctx.exec(editor::make_data_command(
        [&c](auto&) { c += 2; }, [&c](auto&) { c -= 2; }));
      ctx.exec(editor::make_data_command(
        [&c](auto&) { c += 3; }, [&c](auto&) { c -= 3; }));

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
    editor::data_context ctx;
    ctx.exec(editor::make_data_command([](auto&) { throw 42; }, [](auto&) {}));

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
      editor::data_context ctx;
      ctx.exec(editor::make_data_command([&i](auto&) { ++i; }, [](auto&) {}));
      ctx.exec(editor::make_data_command([&i](auto&) { ++i; }, [](auto&) {}));
      {
        auto lck = ctx.lock();
        auto tmp = i;
        ctx.exec(editor::make_data_command([&i](auto&) { ++i; }, [](auto&) {}));
        ctx.exec(editor::make_data_command([&i](auto&) { ++i; }, [](auto&) {}));
        REQUIRE(i == tmp);
      }
    }
    REQUIRE(i == 4);
  }
}