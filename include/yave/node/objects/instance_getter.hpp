//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/primitive.hpp>
#include <yave/core/rts.hpp>

namespace yave {

  /// Helper class template for bind_info's instance getter function.
  template <class T>
  struct InstanceGetterFunction
    : Function<InstanceGetterFunction<T>, Primitive, Object>
  {
    typename InstanceGetterFunction::return_type code() const
    {
      return object_ptr<const Object>(make_object<T>());
    }
  };
}