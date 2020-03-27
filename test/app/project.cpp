//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>
#include <yave/app/project.hpp>
#include <yave/module/std/module_loader.hpp>

using namespace yave;
using namespace yave::app;

TEST_CASE("app::project")
{
  vulkan::vulkan_context vk;
  modules::_std::module_loader loader(vk);

  project_config cfg {
    "",
    {modules::_std::module_name},
    {},
    scene_config(
      100, 100, 60, image_format::rgba32f, audio_format::pcm_44100_stereo)};

  project p(cfg, loader);

  REQUIRE(p.get_name() == "");
  REQUIRE(!p.has_path());
  REQUIRE(p.get_path() == std::nullopt);
  REQUIRE(p.get_scene_config().width() == cfg.scene_cfg.width());
  REQUIRE(p.get_scene_config().height() == cfg.scene_cfg.height());
  REQUIRE(p.get_scene_config().frame_rate() == cfg.scene_cfg.frame_rate());
  REQUIRE(p.get_scene_config().audio_format() == cfg.scene_cfg.audio_format());

  SECTION("")
  {
    p.set_name("abc");
    REQUIRE(p.get_name() == "abc");
  }

  SECTION("")
  {
    scene_config scfg2(
      200, 200, 60, image_format::rgba32f, audio_format::pcm_44100_stereo);
    p.set_scene_config(scfg2);
    REQUIRE(p.get_scene_config().width() == 200);
    REQUIRE(p.get_scene_config().height() == 200);
  }

  SECTION("")
  {
    p.set_path("dev/null");
    REQUIRE(p.has_path());
    REQUIRE(p.get_path() == "dev/null");
  }
}