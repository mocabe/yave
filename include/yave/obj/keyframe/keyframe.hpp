//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/keyframe/keyframe.hpp>
#include <yave/lib/keyframe/float.hpp>
#include <yave/rts/box.hpp>

namespace yave {

  /// Templated keyframe object
  template <class T>
  using Keyframe = Box<keyframe<T>>;

  /// Object type of keyframe<int64_t>
  using KeyframeInt = Keyframe<int64_t>;
  /// Object type of keyframe<kf_float_handles>
  using KeyframeFloat = Keyframe<kf_float_handles>;
  /// Object type of keyframe<bool>
  using KeyframeBool = Keyframe<bool>;

} // namespace yave

YAVE_DECL_TYPE(yave::KeyframeInt, "bf4cb7db-32e8-4bc1-bc9e-c936cb53dac3");
YAVE_DECL_TYPE(yave::KeyframeFloat, "66d7b6cc-167a-4630-87aa-50ac1b156b4e");
YAVE_DECL_TYPE(yave::KeyframeBool, "427bf0f3-ec15-4413-ae3b-964ce41647bf");