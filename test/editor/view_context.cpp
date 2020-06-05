//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/view_context.hpp>
#include <yave/wm/viewport_window.hpp>
#include <yave/wm/root_window.hpp>
#include <catch2/catch.hpp>

#include <yave/editor/button.hpp>

#include <iostream>

using namespace yave;

TEST_CASE("view_context")
{
  vulkan::vulkan_context vk;
  glfw::glfw_context glfw;
  editor::data_context data;

  SECTION("window")
  {
    editor::view_context ctx(data, vk, glfw);

    REQUIRE(ctx.window_manager().should_close());
    ctx.run();

    auto vp = ctx.window_manager().add_viewport(1280, 720, u8"ぽだもし");
    REQUIRE(vp);

    vp->add_window(std::make_unique<editor::button>(
      u8"ぱみだむ", fvec2 {100, 100}, fvec2 {500, 500}));

    // main loop
    ctx.run();
  }
}