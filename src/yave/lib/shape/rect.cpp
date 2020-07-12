//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/shape/rect.hpp>
#include <cmath>

namespace yave {

  auto create_rect_shape(const fvec2& pos, const fvec2& size) -> shape
  {
    path p;
    p.move({1, -1});
    p.line({1, 1});
    p.line({-1, 1});
    p.line({-1, -1});
    p.close();

    shape ret;
    ret.add(std::move(p));
    ret.scale(size.x / 2, size.y / 2);
    ret.translate(pos.x, pos.y);
    return ret;
  }
}