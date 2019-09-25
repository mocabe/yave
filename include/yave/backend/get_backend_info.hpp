//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/backend/backend_info.hpp>

namespace yave {

  template <class BackendTag>
  struct backend_info_traits
  {
    // static object_ptr<const BackendInfo> get_backend_info() { ... }
  };

  /// Get backend info
  template <class BackendTag>
  [[nodiscard]] object_ptr<const BackendInfo> get_backend_info()
  {
    return backend_info<BackendTag>::get_backend_info();
  }
} // namespace yave