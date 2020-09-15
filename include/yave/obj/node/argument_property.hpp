//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/node/argument_property.hpp>
#include <yave/rts/box.hpp>

namespace yave {

  /// Node argument property which contains single initial value
  template <class T>
  using ValueArgumentProperty =
    Box<data::value_argument_property_object_data<std::decay_t<T>>>;

  /// Node argument property which contains numeric value properties
  template <class T>
  using NumericDataTypeProperty =
    Box<data::numeric_argument_property_object_data<std::decay_t<T>>>;
}