//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/shape/shape.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace yave {

  shape::shape(std::vector<path> pths, std::vector<shape_cmd> cmds)
    : m_paths {std::move(pths)}
    , m_commands {std::move(cmds)}
  {
  }

  auto shape::add_path(path p) -> size_t
  {
    m_paths.push_back(p);
    return m_paths.size() - 1;
  }

  void shape::fill(const glm::fvec4& color)
  {
    for (size_t i = 0; i < m_paths.size(); ++i)
      m_commands.push_back({i, {shape_op_fill {.color = color}}});
  }

  void shape::fill(size_t idx, const glm::fvec4& color)
  {
    if (m_paths.size() <= idx)
      throw std::invalid_argument("invalid path index ");

    m_commands.push_back({idx, {shape_op_fill {.color = color}}});
  }

  void shape::stroke(const glm::fvec4& color, float width)
  {
    for (size_t i = 0; i < m_paths.size(); ++i)
      m_commands.push_back(
        {i, {shape_op_stroke {.color = color, .width = width}}});
  }

  void shape::stroke(size_t idx, const glm::fvec4& color, float width)
  {
    if (m_paths.size() <= idx)
      throw std::invalid_argument("invalid path index ");

    m_commands.push_back(
      {idx, {shape_op_stroke {.color = color, .width = width}}});
  }

  void shape::transform(const glm::fmat3& mat)
  {
    auto transform_point = [&mat](const auto& p) {
      auto v = mat * glm::fvec3(glm::fvec2(p.x, p.y), 1.f);
      return glm::fvec2(v.x, v.y);
    };

    for (auto&& path : m_paths)
      for (auto&& p : path.points())
        p = transform_point(p);
  }

  void shape::translate(float x, float y)
  {
    transform(glm::fmat3(glm::translate(glm::fmat3(1.f), glm::vec2(x, y))));
  }

  void shape::rotate(float degree, const glm::fvec2& center)
  {
    auto e   = glm::fmat3(1.f);
    auto t1  = glm::fmat3(glm::translate(e, -center));
    auto t2  = glm::fmat3(glm::translate(e, +center));
    auto rot = glm::fmat3(glm::rotate(e, degree / 180 * glm::pi<float>()));
    transform(t2 * rot * t1);
  }

  void shape::scale(float sx, float sy, const glm::fvec2& center)
  {
    auto e  = glm::fmat3(1.f);
    auto t1 = glm::fmat3(glm::translate(e, -center));
    auto t2 = glm::fmat3(glm::translate(e, +center));
    auto s  = glm::fmat3(glm::scale(glm::fmat3(1.f), glm::vec2(sx, sy)));
    transform(t2 * s * t1);
  }

  void shape::merge(const shape& other)
  {
    auto base = m_paths.size();
    m_paths.insert(m_paths.end(), other.m_paths.begin(), other.m_paths.end());

    m_commands.reserve(m_commands.size() + other.m_commands.size());
    for (auto&& c : other.m_commands) {
      m_commands.emplace_back(c).path_idx += base;
    }
  }
}