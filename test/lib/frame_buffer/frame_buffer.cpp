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
  frame_buffer_manager mng(100, 100, image_format::rgba8, uuid());

  auto pool = mng.get_pool_object();
  REQUIRE(pool);

  auto buff = make_object<FrameBuffer>(pool);
  REQUIRE(buff);
  REQUIRE(buff->view().width() == 100);
  REQUIRE(buff->view().height() == 100);
  REQUIRE(buff->view().data());
  REQUIRE(buff->use_count() == 1);
}

TEST_CASE("1", "[lib][frame_buffer]")
{
  frame_buffer_manager fb_mngr {1920, 1080, image_format::rgba8, uuid()};
  REQUIRE(fb_mngr.format() == image_format::rgba8);
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
  frame_buffer_manager mngr {1920, 1080, image_format::rgba32f, uuid()};
  const auto f1 = frame_buffer(mngr.get_pool_object());
  auto view     = f1.view();
  auto data     = f1.view().data();
  REQUIRE(view.image_format() == mngr.format());
  REQUIRE(view.width() == 1920);
  REQUIRE(view.height() == 1080);
  REQUIRE(view.data() == data);
}

TEST_CASE("3", "[lib][frame_buffer]")
{
  frame_buffer_manager mngr(1920, 1080, image_format::rgba8, uuid());
  const auto f1 = make_object<FrameBuffer>(mngr.get_pool_object());
  auto view     = f1->view();
  REQUIRE(view.image_format() == mngr.format());
  REQUIRE(view.width() == 1920);
  REQUIRE(view.height() == 1080);

  auto f2 = f1.clone();
  REQUIRE(f2->id() != f1->id());

  f2->view().data()[0] = std::byte(255);
  REQUIRE(f2->view().data()[0] == std::byte(255));
  REQUIRE(f1->view().data()[0] != std::byte(255));
}