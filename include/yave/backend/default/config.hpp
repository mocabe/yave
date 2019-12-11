//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/support/uuid.hpp>

namespace yave {

  /// namespace for backend tags.
  namespace backend_tags {

    /// common functions
    struct default_common
    {
    };

    /// backend tag for default render backend
    struct default_render
    {
    };

    /// backend tag for default preview backend
    struct default_preview
    {
    };

  } // namespace backend_tags

  /// For stuff commonly used in default_render/default_preview backends.
  /// Mostly for non-object utilities.
  namespace backends::default_common {

    /// backend id
    constexpr uuid backend_id =
      uuid::from_string("baf89c49-49eb-4381-9b8d-d85b44553090");

  } // namespace backends::default_common

  /// Default renderer backend namespace.
  /// Contains node implementation and binding generators.
  namespace backends::default_render {

    /// backend id
    constexpr uuid backend_id =
      uuid::from_string("f1f12b79-570f-4ec0-9ea2-0ccb16f13a20");

  } // namespace backends::default_render

  /// Default preview backend namespace.
  /// Contains node implementation and binding generators.
  namespace backends::default_preview {

    /// backend id
    constexpr uuid backend_id =
      uuid::from_string("ff10baae-8197-4612-995f-13d5338c200d");

  } // namespace backends::default_preview

} // namespace yave