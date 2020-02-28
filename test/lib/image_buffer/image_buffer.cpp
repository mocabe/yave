//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/lib/image_buffer/image_buffer_manager.hpp>
#include <yave/obj/image_buffer/image_buffer.hpp>

using namespace yave;

TEST_CASE("image_buffer")
{
  image_buffer_manager mngr {uuid()};

  SECTION("create")
  {
    image_format fmt;

    SECTION("rgba8")
    {
      fmt = image_format::rgba8;
    }
    SECTION("rgba16")
    {
      fmt = image_format::rgba16;
    }
    SECTION("rgba32f")
    {
      fmt = image_format::rgba32f;
    }
    SECTION("rgb8")
    {
      fmt = image_format::rgb8;
    }
    SECTION("rgb16")
    {
      fmt = image_format::rgb16;
    }

    auto img = mngr.create(160, 90, fmt);
    REQUIRE(img != uid());
    REQUIRE(mngr.width(img) == 160);
    REQUIRE(mngr.height(img) == 90);
    REQUIRE(mngr.format(img) == fmt);
    REQUIRE(mngr.use_count(img) == 1);
    REQUIRE(mngr.data(img));

    auto bpo = mngr.get_pool_object();
    REQUIRE(bpo);
    REQUIRE(bpo->width(img.data) == mngr.width(img));
    REQUIRE(bpo->height(img.data) == mngr.height(img));
    REQUIRE(bpo->format(img.data) == mngr.format(img));
    REQUIRE(bpo->use_count(img.data) == mngr.use_count(img));
    REQUIRE(bpo->data(img.data) == mngr.data(img));
    REQUIRE(bpo->backend_id() == uuid());

    mngr.ref(img);
    REQUIRE(mngr.use_count(img) == 2);
    mngr.unref(img);
    mngr.unref(img);
  }

  SECTION("create_from")
  {
    auto img1 = mngr.create(160, 90, image_format::rgba8);
    auto img2 = mngr.create_from(img1);

    REQUIRE(mngr.width(img2) == mngr.width(img1));
    REQUIRE(mngr.height(img2) == mngr.height(img1));
    REQUIRE(mngr.format(img2) == mngr.format(img1));
    REQUIRE(mngr.use_count(img2) == mngr.use_count(img1));
    REQUIRE(mngr.data(img2) != mngr.data(img1));
  }

  SECTION("buffer object")
  {
    auto pool = mngr.get_pool_object();
    auto buff = make_object<ImageBuffer>(pool, 190u, 60u, image_format::rgba16);
    REQUIRE(buff);
    REQUIRE(buff->view().width() == 190);
    REQUIRE(buff->view().height() == 60);

    auto view = buff->view();
    std::fill_n(view.data(), view.byte_size(), std::byte {42});

    auto buff2 = buff.clone();
    REQUIRE(buff2);
    REQUIRE(buff2->use_count() == 1);
    REQUIRE(buff2->view().width() == 190);
    REQUIRE(buff2->view().height() == 60);
  }
}
