//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>
#include <yave/lib/frame_buffer/frame_buffer.hpp>

using namespace yave;

TEST_CASE("", "[lib][frame_buffer]")
{
  frame_buffer_manager mng(-1, -1, image_format::RGBA8UI);
}

TEST_CASE("1", "[lib][frame_buffer]")
{
  frame_buffer_manager fb_mngr {1920, 1080, image_format::RGBA8UI};
  REQUIRE(fb_mngr.format() == image_format::RGBA8UI);
  REQUIRE(fb_mngr.width() == 1920);
  REQUIRE(fb_mngr.height() == 1080);
  auto fb1 = fb_mngr.create();
  auto fb2 = fb_mngr.create_from(fb1);
  REQUIRE(fb1 != fb2);
  auto fb3 = fb_mngr.create_from(fb2);
  REQUIRE(fb2 != fb3);
  fb_mngr.unref(fb1);
  fb_mngr.unref(fb2);
  fb_mngr.unref(fb3);
}

TEST_CASE("2", "[lib][frame_buffer]")
{
  frame_buffer_manager mngr {1920, 1080, image_format::RGBA32F};
  const auto f1 = frame_buffer(mngr.get_pool_object());
  auto view     = f1.get_image_view();
  auto data     = f1.get_image_view().data();
  REQUIRE(view.image_format() == mngr.format());
  REQUIRE(view.width() == 1920);
  REQUIRE(view.height() == 1080);
  REQUIRE(view.data() == data);
}

TEST_CASE("3", "[lib][frame_buffer]")
{
  frame_buffer_manager mngr(1920, 1080, image_format::RGBA32F);
  const auto f1 = make_object<FrameBuffer>(mngr.get_pool_object());
  auto view     = f1->get_image_view();
  REQUIRE(view.image_format() == mngr.format());
  REQUIRE(view.width() == 1920);
  REQUIRE(view.height() == 1080);
  auto f2                        = f1->copy();
  f2->get_image_view().data()[0] = 255;
}