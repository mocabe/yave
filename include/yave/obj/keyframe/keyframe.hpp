//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/keyframe/alias.hpp>
#include <yave/rts/box.hpp>

namespace yave {

  /// Object type of keyframe<int64_t>
  using KeyframeDataInt = Box<kf_int_t>;
  /// Object type of keyframe<kf_float_handles>
  using KeyframeDataFloat = Box<kf_float_t>;
  /// Object type of keyframe<bool>
  using KeyframeDataBool = Box<kf_bool_t>;

} // namespace yave

YAVE_DECL_TYPE(yave::KeyframeDataInt, "bf4cb7db-32e8-4bc1-bc9e-c936cb53dac3");
YAVE_DECL_TYPE(yave::KeyframeDataFloat, "66d7b6cc-167a-4630-87aa-50ac1b156b4e");
YAVE_DECL_TYPE(yave::KeyframeDataBool, "427bf0f3-ec15-4413-ae3b-964ce41647bf");