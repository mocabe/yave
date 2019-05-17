//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/rts/box.hpp>
#include <yave/core/data_types/frame.hpp>

namespace yave {

  /// Frame object
  using Frame = Box<frame>;

} // namespace yave

YAVE_DECL_TYPE(yave::Frame, "510a0f3a-8c18-46f9-9cc6-514609e35c13");