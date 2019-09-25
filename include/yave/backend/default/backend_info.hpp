//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/backend/get_backend_info.hpp>

#include <yave/backend/default/render/system.hpp>

namespace yave {

  template <>
  struct backend_info_traits<backend::tags::default_render>
  {
    static object_ptr<const BackendInfo> get_backend_info()
    {
      static backend::default_render::backend b;
      return b.get_backend_info();
    }
  };
} // namespace yave
