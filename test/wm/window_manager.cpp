//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window_manager.hpp>
#include <catch2/catch.hpp>

#include <iostream>

using namespace yave;

TEST_CASE("window_manager")
{
  SECTION("init")
  {
    wm::window_manager wm;
  }

  SECTION("w")
  {
    struct win : wm::window
    {
      win()
        : window("test window", {}, {})
      {
      }

      void render(editor::render_context&) const override
      {
      }
      void resize(const fvec2&, const fvec2&) override
      {
      }
      void update(editor::data_context&, editor::view_context&) override
      {
      }
    };

    wm::window_manager wm;

    REQUIRE(!wm.add_window({}, 0, wm::make_window<win>()));

    REQUIRE(wm.add_window(wm.root()->id(), 0, wm::make_window<win>()));
    REQUIRE(wm.root()->children().size() == 1);
    REQUIRE(wm.root()->children()[0]->parent() == wm.root());

    wm.remove_window(wm.root()->id()); // fail
    REQUIRE(wm.exists(wm.root()->id()));

    auto cid = wm.root()->children()[0]->id();
    wm.remove_window(wm.root()->children()[0]->id());
    REQUIRE(wm.root()->children().empty());
    REQUIRE(!wm.exists(cid));
  }
}