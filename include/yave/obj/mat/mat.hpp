//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/data/mat/mat.hpp>

namespace yave {

  using Mat2 = Box<data::mat2>;
  using Mat3 = Box<data::mat3>;
  using Mat4 = Box<data::mat4>;

} // namespace yave

YAVE_DECL_TYPE(yave::Mat2, "035f8b34-8f2b-4e79-9ebf-319c4a029cb6");
YAVE_DECL_TYPE(yave::Mat3, "f66a0e02-e3b8-4835-969b-e64ce4c18640");
YAVE_DECL_TYPE(yave::Mat4, "fda0796a-473c-4f4d-bba8-535011a934de");