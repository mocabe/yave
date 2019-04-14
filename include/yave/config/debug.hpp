//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

namespace yave {

#if defined(NDEBUG)
  constexpr bool is_debug = false;
#else
  constexpr bool is_debug = true;
#endif

}