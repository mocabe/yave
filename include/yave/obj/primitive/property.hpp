//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/node/argument_property.hpp>

namespace yave {

  using BoolDataProperty = ValueArgumentProperty<Bool>;
  YAVE_DECL_NODE_ARGUMENT_PROPERTY(Bool, Bool, BoolDataProperty);

  using StringDataProperty = ValueArgumentProperty<String>;
  YAVE_DECL_NODE_ARGUMENT_PROPERTY(String, String, StringDataProperty);

  using FloatDataProperty = NumericDataTypeProperty<Float>;
  YAVE_DECL_NODE_ARGUMENT_PROPERTY(Float, Float, FloatDataProperty);

  using IntDataProperty = NumericDataTypeProperty<Int>;
  YAVE_DECL_NODE_ARGUMENT_PROPERTY(Int, Int, IntDataProperty);

} // namespace yave

YAVE_DECL_TYPE(yave::IntDataProperty, "d60f1ac7-5a57-4037-9d78-c9805cbe5407");
YAVE_DECL_TYPE(yave::FloatDataProperty, "621188d3-d162-4778-aedd-3be0a4745c3e");
YAVE_DECL_TYPE(yave::BoolDataProperty, "95a0e534-273b-4911-957d-0902f8d769a4");
YAVE_DECL_TYPE(yave::StringDataProperty, "6d7e77a5-3cd3-4bd5-bd18-e9b1a02cb494");