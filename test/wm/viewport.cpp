//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/view_context.hpp>
#include <yave/wm/viewport.hpp>

#include <catch2/catch.hpp>
#include <iostream>

using namespace yave;

TEST_CASE("viewport")
{
  editor::data_context dctx;
  editor::view_context vctx {dctx};

  glfw::glfw_context glfw;

  auto root = vctx.window_manager().root();
  auto vp   = root->add_viewport(std::make_unique<wm::viewport>(
    vctx.window_manager(), glfw.create_window(1280, 720, "test window")));

  // vctx.window_manager().set_key_focus(vp);
  vctx.run();
}