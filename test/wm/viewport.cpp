//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/view_context.hpp>
#include <yave/wm/viewport.hpp>
#include <yave/wm/root_window.hpp>

#include <catch2/catch.hpp>
#include <iostream>

using namespace yave;

TEST_CASE("viewport")
{
  editor::data_context dctx;
  editor::view_context vctx {dctx};

  glfw::glfw_context glfw;
  vulkan::vulkan_context vk;

  auto& wm = vctx.window_manager();

  auto root = wm.root();
  auto vp   = root
              ->add_viewport(std::make_unique<wm::viewport>(
                wm, vk, glfw.create_window(1280, 720, "test window")))
              ->as<wm::viewport>();

  wm.set_key_focus(vp);

  REQUIRE(wm.get_key_focus() == vp);
  REQUIRE(wm.get_window(root->id()) == root);
  REQUIRE(wm.get_window(vp->id()) == vp);
  REQUIRE(!wm.get_window(uid()));
  REQUIRE(!wm.get_viewport(root->id()));
  REQUIRE(!wm.get_viewport(uid()));
  REQUIRE(wm.get_viewport(vp->id()) == vp);
  REQUIRE(!vp->layout());
  REQUIRE(vp->modals().empty());
  REQUIRE(!vp->modal(uid()));

  vctx.run();
}