
#include <catch2/catch.hpp>
#include <yave/frame_buffer/frame_buffer.hpp>
#include <yave/frame_buffer/frame_buffer_manager.hpp>

using namespace yave;

TEST_CASE("frame_buffer")
{
  SECTION("manager")
  {
    frame_buffer_manager fb_mngr(image_format::RGBA8UI, 1920, 1080);
    auto fb1 = fb_mngr.create();
    auto fb2 = fb_mngr.create(fb1);
    REQUIRE(fb1 != fb2);
    auto fb3 = fb_mngr.create(fb_mngr.get_data(fb2));
    REQUIRE(fb2 != fb3);
    fb_mngr.unref(fb1);
    fb_mngr.unref(fb2);
    fb_mngr.unref(fb3);
    REQUIRE(fb_mngr.size() == 0);
  }
}