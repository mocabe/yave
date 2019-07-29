//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

namespace yave {

  namespace backend::tags {

    /// backend tag for default render backend
    struct default_render;

    /// backend tag for default preview backend
    using default_preview = default_render;

  } // namespace backend::tags

} // namespace yave