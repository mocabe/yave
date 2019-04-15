//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/type_gen.hpp>

namespace yave {

  /// Undefined
  ///
  /// null objects are treated as instances of Undefined.
  struct Undefined;

} // namespace yave

template <>
struct yave::object_type_traits<yave::Undefined>
{
  static constexpr const char name[] = "_Undefined(NULL)";
};