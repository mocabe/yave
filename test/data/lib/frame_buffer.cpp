//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/data/lib/frame_buffer.hpp>
#include <yave/data/obj/frame_buffer.hpp>

#include <yave/backend/default/system/frame_buffer_manager.hpp>

using namespace yave;
using namespace yave::backend::default_common;

TEST_CASE("frame_buffer")
{
  SECTION("manager")
  {
    SECTION("basic")
    {
      frame_buffer_manager fb_mngr {1920, 1080, image_format::RGBA8UI};
      REQUIRE(fb_mngr.format() == image_format::RGBA8UI);
      REQUIRE(fb_mngr.width() == 1920);
      REQUIRE(fb_mngr.height() == 1080);
      REQUIRE(fb_mngr.size() == 0);
      auto fb1 = fb_mngr.create();
      REQUIRE(fb_mngr.size() == 1);
      auto fb2 = fb_mngr.create(fb1);
      REQUIRE(fb1 != fb2);
      REQUIRE(fb_mngr.size() == 2);
      auto fb3 = fb_mngr.create(fb_mngr.get_data(fb2));
      REQUIRE(fb2 != fb3);
      REQUIRE(fb_mngr.size() == 3);
      fb_mngr.unref(fb1);
      fb_mngr.unref(fb2);
      fb_mngr.unref(fb3);
      REQUIRE(fb_mngr.size() == 0);
    }
  }

  SECTION("value")
  {
    frame_buffer_manager mngr {1920, 1080, image_format::RGBA32F};
    const auto f1 = frame_buffer(mngr.get_pool_object());
    REQUIRE(mngr.size() == 1);
    auto view = f1.get_image_view();
    auto data = f1.get_image_view().data();
    REQUIRE(view.image_format() == mngr.format());
    REQUIRE(view.width() == 1920);
    REQUIRE(view.height() == 1080);
    REQUIRE(view.data() == data);
    REQUIRE(mngr.size() == 1);
  }

  SECTION("obj")
  {
    frame_buffer_manager mngr(1920, 1080, image_format::RGBA32F);
    const auto f1 = make_object<FrameBuffer>(mngr.get_pool_object());
    REQUIRE(mngr.size() == 1);
    auto view = f1->get_image_view();
    REQUIRE(view.image_format() == mngr.format());
    REQUIRE(view.width() == 1920);
    REQUIRE(view.height() == 1080);
    auto f2 = f1->copy();
    REQUIRE(mngr.size() == 2);
    f2->get_image_view().data()[0] = 255;
  }
}