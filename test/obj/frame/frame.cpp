//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/obj/frame_time/frame_time.hpp>

using namespace yave;

TEST_CASE("frame")
{
  SECTION("make_object")
  {
    [[maybe_unused]] auto v0 = make_object<const FrameTime>();
    [[maybe_unused]] auto v1 = make_object<const FrameTime>(0, 0);
    [[maybe_unused]] auto v2 = make_object<const FrameTime>(frame_time());
    [[maybe_unused]] auto v3 = make_object<const FrameTime>(time::zero(), 0);
  }
  SECTION("access")
  {
    [[maybe_unused]] auto v = make_object<const FrameTime>()->time_point;
  }
}