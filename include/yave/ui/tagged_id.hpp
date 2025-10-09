//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/typedefs.hpp>

#include <random>

namespace yave::ui {

  /// Tagged id type
  template <class T>
  struct tagged_id
  {
    u64 data = 0;

    auto operator<=>(const tagged_id&) const = default;

    /// random gen
    static auto random() -> tagegd_id
    {
      static auto rd = std::random_device();
      static auto rn = std::mt19937_64(rd());
      return {rn()};
    }
  };

} // namespace yave::ui