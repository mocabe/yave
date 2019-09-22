//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/backend/backend_info.hpp>

namespace yave{

  template <class Backend>
  struct backend_info_traits
  {
    // static backend_info get_backend_info() { /* return your backend info */ }
  };

  /// Get backend info
  template <class Backend>
  backend_info get_backend_info()
  {
    return backend_info<Backend>::get_backend_info();
  }
}