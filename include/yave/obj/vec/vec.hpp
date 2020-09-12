//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/data/vec/vec.hpp>

namespace yave {

  using Vec1 = Box<data::vec1>;
  using Vec2 = Box<data::vec2>;
  using Vec3 = Box<data::vec3>;
  using Vec4 = Box<data::vec4>;

} // namespace yave

YAVE_DECL_TYPE(yave::Vec1, "9249315c-a062-4545-9063-6ed80df95d5e");
YAVE_DECL_TYPE(yave::Vec2, "a3b74ed6-ae4f-437a-ad58-d6ff679cc25c");
YAVE_DECL_TYPE(yave::Vec3, "d0a25c41-e9b8-4ff8-a1dd-ca9687087213");
YAVE_DECL_TYPE(yave::Vec4, "90e4a48f-4d22-449e-911f-ec0627aa2009");