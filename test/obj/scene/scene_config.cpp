//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/scene/scene_config.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("init", "[obj][scene][scene_config]")
{
  REQUIRE(make_object<SceneConfig>(
    1920U, 1080U, 60U, image_format::rgba8, audio_format::pcm_44100_mono));
}