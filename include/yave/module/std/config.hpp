//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/support/uuid.hpp>

namespace yave {

  /// std
  namespace modules::_std {

    /// tag
    struct tag;

    /// backend id
    constexpr uuid module_id =
      uuid::from_string("baf89c49-49eb-4381-9b8d-d85b44553090");

    constexpr auto module_name = "std";

  } // namespace modules::_std

} // namespace yave