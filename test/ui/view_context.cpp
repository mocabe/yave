//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/view_context.hpp>
#include <yave/ui/root.hpp>
#include <yave/ui/viewport.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/application.hpp>
#include <catch2/catch.hpp>

#include <iostream>
#include <future>

using namespace yave::ui;

TEST_CASE("vctx")
{
  main_context mctx;
  data_context dctx;
  view_context vctx(mctx, dctx);

  SECTION("")
  {
    int i = 0;

    // initial task
    vctx.post([&](auto&) { ++i; });

    SECTION("")
    {
      vctx.post([&](auto&) { vctx.exit(); });
    }

    SECTION("")
    {
      vctx.post([](auto& v) { v.exit(); });
    }

    SECTION("")
    {
      vctx.post([](auto& v) {
        using namespace std::chrono_literals;
        std::cout << "waiting..." << std::endl;
        std::this_thread::sleep_for(10ms);
        v.exit();
      });
    }

    mctx.post([&](auto&) { vctx.run(); });
    mctx.run();

    REQUIRE(i == 1);
  }

  SECTION("")
  {
    int i = 0;
    vctx.post([&](auto&) { i = 1; });
    vctx.post([&](auto&) { i *= 2; });
    vctx.post_delay([&](auto&) { vctx.exit(); });

    mctx.post([&](auto&) { vctx.run(); });
    mctx.run();

    REQUIRE(i == 2);
  }

  SECTION("")
  {
    int i = 0;
    vctx.post_delay([&](auto&) { i = 1; });
    vctx.post([&](auto&) { i *= 2; });
    vctx.post_delay([&](auto&) { vctx.exit(); });

    mctx.post([&](auto&) { vctx.run(); });
    mctx.run();

    REQUIRE(i == 1);
  }

  // SECTION("wm")
  // {
  //   vctx.post([](auto& vctx) {
  //     auto& wm = vctx.window_manager();
  //     auto vp  = wm.root()->add_viewport(u8"テスト", size(500, 500));
  //     vp->get_native()->show();
  //   });

  //   mctx.post([&](auto&) { vctx.run(); });
  //   mctx.run();
  // }

  SECTION("app")
  {
    struct TestApp : application
    {
      void init(view_context& vctx) override
      {
        auto& wm = vctx.window_manager();
        auto& vp = wm.root().add_viewport(u8"テスト", size(500, 500));
        vp.native_window().show();
      }
    };

    TestApp app;
    app.run();
  }
}