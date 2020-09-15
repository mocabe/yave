//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/obj/node/argument_property.hpp>

namespace yave {

  /// data property
  using TimeDataProperty = ValueArgumentProperty<FrameTime>;
  YAVE_DECL_NODE_ARGUMENT_PROPERTY(FrameTime, FrameTime, TimeDataProperty);
} // namespace yave

YAVE_DECL_TYPE(yave::TimeDataProperty, "c3ef0d0b-d388-477e-b7d9-139ecaa1c001");