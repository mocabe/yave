//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/dynamic_typing.hpp>

namespace yave {

  /// Anti-unification.
  /// \param ts Not-empty list of types
  object_ptr<const Type>
    generalize(const std::vector<object_ptr<const Type>>& ts);

  /// specializable
  bool specializable(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2);

} // namespace yave