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
  data_context dctx;
  view_context vctx(dctx);

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

    vctx.run();

    REQUIRE(i == 1);
  }

  SECTION("")
  {
    int i = 0;
    vctx.post([&](auto&) { i = 1; });
    vctx.post([&](auto&) { i *= 2; });
    vctx.post_delay([&](auto&) { vctx.exit(); });

    vctx.run();

    REQUIRE(i == 2);
  }

  SECTION("")
  {
    int i = 0;
    vctx.post_delay([&](auto&) { i = 1; });
    vctx.post([&](auto&) { i *= 2; });
    vctx.post_delay([&](auto&) { vctx.exit(); });

    vctx.run();

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
    struct my_window final : windowT<my_window>
    {
      void layout(layout_scope ctx) const override
      {
        return ctx.set_size(ctx.constraints().max());
      }

      void render(render_scope ctx) const override
      {
        auto& layer = ctx.get_layer();
        layer.draw_rect(ctx.window_size(), 5, color {.g = 1.0, .a = 1.0});
      }
    };

    struct TestApp : application
    {
      void init(view_context& vctx) override
      {
        auto& wm = vctx.window_manager();
        auto& vp = wm.root().add_viewport(u8"テスト", size(500, 500));

        vp.get_tracker();
        vp.native_window().show();

        vp.set_child(make_unique<my_window>()).show();
      }
    };

    TestApp app;
    app.run();
  }
}