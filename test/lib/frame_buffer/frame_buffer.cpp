//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>
#include <yave/lib/frame_buffer/frame_buffer.hpp>

using namespace yave;

TEST_CASE("")
{
  vulkan::vulkan_context vctx;
  vulkan::offscreen_context osctx(vctx);

  SECTION("", "[lib][frame_buffer]")
  {
    frame_buffer_manager mng(100, 100, image_format::rgba32f, uuid(), osctx);

    auto pool = mng.get_pool_object();
    REQUIRE(pool);

    auto buff = make_object<FrameBuffer>(pool);
    REQUIRE(buff);
    REQUIRE(buff->width() == 100);
    REQUIRE(buff->height() == 100);
    REQUIRE(buff->format() == image_format::rgba32f);
    REQUIRE(buff->use_count() == 1);
  }

  SECTION("1", "[lib][frame_buffer]")
  {
    frame_buffer_manager fb_mngr(
      100, 100, image_format::rgba32f, uuid(), osctx);

    REQUIRE(fb_mngr.format() == image_format::rgba32f);
    REQUIRE(fb_mngr.width() == 100);
    REQUIRE(fb_mngr.height() == 100);
    auto fb1 = fb_mngr.create();
    auto fb2 = fb_mngr.create_from(fb1);
    REQUIRE(fb1 != fb2);
    auto fb3 = fb_mngr.create_from(fb2);
    REQUIRE(fb2 != fb3);
    fb_mngr.unref(fb1);
    fb_mngr.unref(fb2);
    fb_mngr.unref(fb3);
  }
}