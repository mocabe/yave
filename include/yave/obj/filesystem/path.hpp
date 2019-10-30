//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/filesystem/path.hpp>

namespace yave {

  /// Frame buffer object
  using FilesystemPath = Box<filesystem::path>;

} // namespace yave

YAVE_DECL_TYPE(yave::FilesystemPath, "6b794215-3cb6-4012-847a-6ad9f70df76f");