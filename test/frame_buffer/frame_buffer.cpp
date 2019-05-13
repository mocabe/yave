//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>
#include <yave/frame_buffer/frame_buffer.hpp>
#include <yave/frame_buffer/frame_buffer_manager.hpp>
#include <yave/obj/frame_buffer.hpp>

using namespace yave;

TEST_CASE("frame_buffer")
{
  SECTION("manager")
  {
    SECTION("basic")
    {
      frame_buffer_manager fb_mngr {image_format::RGBA8UI, 1920, 1080};
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
    frame_buffer_manager mngr {image_format::RGBA32F, 1920, 1080};
    const auto f1 = frame_buffer(mngr);
    REQUIRE(mngr.size() == 1);
    auto view = f1.get_image_view();
    auto data = f1.data();
    REQUIRE(view.image_format() == mngr.format());
    REQUIRE(view.width() == 1920);
    REQUIRE(view.height() == 1080);
    REQUIRE(view.data() == data);
    REQUIRE(mngr.size() == 1);
  }

  SECTION("obj")
  {
    frame_buffer_manager mngr(image_format::RGBA8UI, 1920, 1080);
    const auto f1 = make_object<FrameBuffer::FrameBuffer>(mngr);
    REQUIRE(mngr.size() == 1);
    auto view = f1->get_image_view();
    REQUIRE(view.image_format() == mngr.format());
    REQUIRE(view.width() == 1920);
    REQUIRE(view.height() == 1080);
    auto f2 = f1->get();
    REQUIRE(mngr.size() == 2);
    f2->data()[0] = 255;
  }
}