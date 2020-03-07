//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/draw2d.hpp>

namespace yave::vulkan {

  namespace {
    auto line_offset(const glm::vec2& p1, const glm::vec2& p2, float d)
    {
      auto v = normalize(p2 - p1);
      auto n = glm::vec2(v.y, -v.x);
      return std::pair {p1 + d * n, p2 + d * n};
    }

    auto line_intersection(
      const glm::vec2& p1,
      const glm::vec2& p2,
      const glm::vec2& q1,
      const glm::vec2& q2)
    {
      auto u = p2 - p1; /* P = p1 + t u */
      auto v = q2 - q1; /* Q = q1 + s v */
      auto n = glm::vec2(u.y, -u.x);
      auto s = dot(n, p1 - q1) / dot(n, v);
      return q1 + s * v;
    }
  } // namespace

  void draw2d_list::add_polyline(
    const glm::vec2* ptr,
    const size_t& size,
    const glm::vec4& col,
    const float& thickness,
    const polyline_flags& flags,
    const draw2d_clip& clip,
    const draw2d_tex& tex)
  {
    if (size < 2)
      return;

    if (thickness < 1)
      return;

    uint32_t vtx_off = vtx_buffer.size();
    uint32_t idx_off = idx_buffer.size();

    auto closed = !!(flags & polyline_flags::closed);

    uint32_t vtx_size = closed ? 2 * (size + 1) : 2 * size;
    uint32_t idx_size = closed ? size * 6 : (size - 1) * 6;

    cmd_buffer.push_back({idx_size, idx_off, vtx_off, tex, clip});

    vtx_buffer.reserve(vtx_buffer.size() + vtx_size);
    idx_buffer.reserve(idx_buffer.size() + idx_size);

    auto d = thickness / 2;

    // calcualte intersection between offset lines
    auto _offset_intersec = [](auto& p1, auto& p2, auto& p3, float d) {
      auto [q1, q2] = line_offset(p1, p2, d);
      auto [r1, r2] = line_offset(p2, p3, d);
      return line_intersection(q1, q2, r1, r2);
    };

    // start of polyline
    vtx_buffer.push_back(
      {line_offset(ptr[0], ptr[1], -d).first, glm::vec2(), col});
    vtx_buffer.push_back(
      {line_offset(ptr[0], ptr[1], +d).first, glm::vec2(), col});

    for (size_t i = 1; i < size - 1; ++i) {
      auto& p1 = ptr[i - 1];
      auto& p2 = ptr[i];
      auto& p3 = ptr[i + 1];
      vtx_buffer.push_back(
        {_offset_intersec(p1, p2, p3, -d), glm::vec2(), col});
      vtx_buffer.push_back(
        {_offset_intersec(p1, p2, p3, +d), glm::vec2(), col});
    }

    if (closed) {
      {
        auto& p1 = ptr[size - 2];
        auto& p2 = ptr[size - 1];
        auto& p3 = ptr[0];
        vtx_buffer.push_back(
          {_offset_intersec(p1, p2, p3, -d), glm::vec2(), col});
        vtx_buffer.push_back(
          {_offset_intersec(p1, p2, p3, +d), glm::vec2(), col});
      }
      {
        auto& p1 = ptr[size - 1];
        auto& p2 = ptr[0];
        auto& p3 = ptr[1];
        {
          auto intersec = _offset_intersec(p1, p2, p3, -d);
          vtx_buffer.push_back({intersec, glm::vec2(), col});
          vtx_buffer[vtx_off + 0].pos = intersec;
        }
        {
          auto intersec = _offset_intersec(p1, p2, p3, +d);
          vtx_buffer.push_back({intersec, glm::vec2(), col});
          vtx_buffer[vtx_off + 1].pos = intersec;
        }
      }
    } else {
      vtx_buffer.push_back(
        {line_offset(ptr[size - 2], ptr[size - 1], -d).second,
         glm::vec2(),
         col});
      vtx_buffer.push_back(
        {line_offset(ptr[size - 2], ptr[size - 1], +d).second,
         glm::vec2(),
         col});
    }

    for (size_t i = 1; i < (closed ? size + 1 : size); ++i) {
      auto i1 = (i - 1) * 2;
      auto i2 = i * 2;
      idx_buffer.push_back({(uint16_t)(i1 + 1)});
      idx_buffer.push_back({(uint16_t)(i1)});
      idx_buffer.push_back({(uint16_t)(i2 + 1)});
      idx_buffer.push_back({(uint16_t)(i1)});
      idx_buffer.push_back({(uint16_t)(i2)});
      idx_buffer.push_back({(uint16_t)(i2 + 1)});
    }
  }

  void draw2d_list::add_polygon(
    const glm::vec2* ptr,
    const size_t& size,
    const glm::vec4& col,
    const polygon_flags&,
    const draw2d_clip& clip,
    const draw2d_tex& tex)
  {
    if (size < 3)
      return;

    uint32_t vtx_off  = vtx_buffer.size();
    uint32_t idx_off  = idx_buffer.size();
    uint32_t vtx_size = size;
    uint32_t idx_size = (size - 2) * 3;

    cmd_buffer.push_back({idx_size, idx_off, vtx_off, tex, clip});

    vtx_buffer.reserve(vtx_buffer.size() + vtx_size);
    idx_buffer.reserve(idx_buffer.size() + idx_size);

    for (size_t i = 0; i < vtx_size; ++i)
      vtx_buffer.push_back({ptr[i], glm::vec2(), col});

    for (size_t i = 2; i < vtx_size; ++i) {
      idx_buffer.push_back({(uint16_t)(0)});
      idx_buffer.push_back({(uint16_t)(i - 1)});
      idx_buffer.push_back({(uint16_t)(i)});
    }
  }
} // namespace yave::vulkan