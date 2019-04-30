//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/tools/id.hpp>

#include <atomic>

namespace yave {

  namespace {
    /// Globel counter for id.
    std::atomic<uint64_t> g_current_id = {};
  } // namespace

  uint64_t get_next_id() noexcept
  {
    return ++g_current_id;
  }

} // namespace yave