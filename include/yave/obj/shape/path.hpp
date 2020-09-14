//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/data/shape/path.hpp>

namespace yave {
  /// path data
  using Path = Box<data::path>;
} // namespace yave

YAVE_DECL_TYPE(yave::Path, "db1b440d-a6c9-4c9b-9fa4-2e6c1ec62be7");