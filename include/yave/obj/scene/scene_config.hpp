//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/scene/scene_config.hpp>

namespace yave {
  /// Time object
  using SceneConfig = Box<scene_config>;
} // namespace yave

YAVE_DECL_TYPE(yave::SceneConfig, "1fcb5c92-272b-43c8-b1c1-cf3bc901128d");