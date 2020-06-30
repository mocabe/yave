//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/shape/circle.hpp>
#include <cmath>

namespace yave {

  auto create_circle_shape(const fvec2& center, float radius) -> shape
  {
    const auto k = 4.f * (std::sqrt(2.f) - 1.f) / 3.f;

    // add circle path
    path p;
    p.line({0, 1});
    p.cubic({k, 1}, {1, k}, {1, 0});
    p.cubic({1, -k}, {k, -1}, {0, -1});
    p.cubic({-k, -1}, {-1, -k}, {-1, 0});
    p.cubic({-1, k}, {-k, 1}, {0, 1});
    p.close();

    shape ret;
    ret.add(std::move(p));
    ret.scale(radius, radius);
    ret.translate(center.x, center.y);
    return ret;
  }
}