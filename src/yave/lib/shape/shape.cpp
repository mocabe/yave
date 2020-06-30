//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/shape/shape.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace yave {

  shape::shape(std::vector<path> paths)
    : m_paths {std::move(paths)}
    , m_transform {1.f}
  {
  }

  void shape::add(path p)
  {
    m_paths.push_back(p);
  }

  void shape::apply_transform()
  {
    auto transform_point = [&](auto& p) {
      auto v = m_transform * fvec3(p, 1.f);
      if (v.z == 0.f)
        p = {0, 0};
      else
        p = {v.x / v.z, v.y / v.z};
    };

    for (auto&& path : m_paths)
      for (auto&& p : path.points())
        transform_point(p);

    m_transform = fmat3(1.f);
  }

  void shape::transform(const fmat3& mat)
  {
    m_transform = mat * m_transform;
  }

  void shape::translate(float x, float y)
  {
    transform(fmat3(glm::translate(glm::mat3x3(1.f), glm::vec2(x, y))));
  }

  void shape::rotate(float degree, const fvec2& center)
  {
    auto e   = glm::mat3x3(1.f);
    auto t1  = fmat3(glm::translate(e, -center));
    auto t2  = fmat3(glm::translate(e, +center));
    auto rot = fmat3(glm::rotate(e, degree));
    transform(t2 * rot * t1);
  }

  void shape::scale(float sx, float sy)
  {
    transform(fmat3(glm::scale(glm::mat3x3(1.f), glm::vec2(sx, sy))));
  }

  auto merge(const shape& s1, const shape& s2) -> shape
  {
    auto s1t = s1;
    auto s2t = s2;
    s1t.apply_transform();
    s2t.apply_transform();

    shape ret;

    for (auto&& path : s1t.paths())
      ret.add(path);
    for (auto&& path : s2t.paths())
      ret.add(path);
    return ret;
  }
}