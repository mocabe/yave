//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/draw.hpp>

#include <utility>
#include <numbers>
#include <iostream>

namespace yave::wm {

  namespace {

    /// convert f32 color to u8 normalized color, assuming each normalized color
    /// component is centered around i/(2^B-1).
    auto u8col(glm::vec4 col)
    {
      return glm::u8vec4(col * 255.f + 0.5f);
    }

    /// clip scissor to positive quadrant (required for Vulkan API)
    auto clip_scissor(const draw_scissor& scissor)
    {
      assert(0 <= scissor.extent.x && 0 <= scissor.extent.y);

      // clip offset
      auto off = glm::max(glm::vec2(0, 0), scissor.offset);

      return draw_scissor {
        .offset = off,
        .extent = scissor.offset + scissor.extent - off,
      };
    }

    /// calculate offset of line segment
    auto line_offset(const glm::vec2& p1, const glm::vec2& p2, float d)
      -> std::pair<glm::vec2, glm::vec2>
    {
      auto v = normalize(p2 - p1);
      auto n = glm::vec2(v.y, -v.x);
      return std::pair {p1 + d * n, p2 + d * n};
    }

    /// calculate intersection of two lines
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

    /// reserve behaviour for buffers
    template <class T>
    void reserve_increase(std::vector<T>& vec, size_t increase)
    {
      // avoid unnecessary reserve when natural capacity growth is enough
      if (vec.size() + increase > 3 * vec.capacity() / 2)
        vec.reserve(vec.size() + increase);
    }

    enum polyline_flags
    {
      none   = 0,
      closed = 1,
    };

    void dl__draw_polyline(
      draw_list& dl,
      const std::span<glm::vec2>& ps,
      const float& width,
      const polyline_flags& flags,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex)
    {
      auto& vtx_buff = dl.vtx_buffer;
      auto& idx_buff = dl.idx_buffer;
      auto& cmd_buff = dl.cmd_buffer;

      auto sz = ps.size();

      if (sz < 2)
        return;

      if (width < 0)
        return;

      uint32_t vtx_off = vtx_buff.size();
      uint32_t idx_off = idx_buff.size();

      auto closed = !!(flags & polyline_flags::closed);

      uint32_t vtx_size = closed ? 2 * (sz + 1) : 2 * sz;
      uint32_t idx_size = closed ? sz * 6 : (sz - 1) * 6;

      cmd_buff.push_back(
        {.idx_count  = idx_size,
         .idx_offset = idx_off,
         .vtx_offset = vtx_off,
         .tex        = tex,
         .scissor    = clip_scissor(scissor)});

      reserve_increase(vtx_buff, vtx_size);
      reserve_increase(idx_buff, idx_size);

      auto d = width / 2;

      // calcualte intersection between offset lines
      constexpr auto _offset_intersec = [](const auto& off1, const auto& off2) {
        auto& [q1, q2] = off1;
        auto& [r1, r2] = off2;
        return line_intersection(q1, q2, r1, r2);
      };

      // start of polyline
      vtx_buff.push_back(
        {.pos = line_offset(ps[0], ps[1], -d).first,
         .uv  = {},
         .col = u8col(col)});
      vtx_buff.push_back(
        {.pos = line_offset(ps[0], ps[1], +d).first,
         .uv  = {},
         .col = u8col(col)});

      // previous line offset -d/+d
      auto offm = line_offset(ps[0], ps[1], -d);
      auto offp = line_offset(ps[0], ps[1], +d);

      for (size_t i = 1; i < sz - 1; ++i) {
        auto& p2 = ps[i];
        auto& p3 = ps[i + 1];

        auto offm1 = offm;
        auto offm2 = line_offset(p2, p3, -d);
        auto offp1 = offp;
        auto offp2 = line_offset(p2, p3, +d);

        vtx_buff.push_back(
          {.pos = _offset_intersec(offm1, offm2), .uv = {}, .col = u8col(col)});
        vtx_buff.push_back(
          {.pos = _offset_intersec(offp1, offp2), .uv = {}, .col = u8col(col)});

        offm = offm2;
        offp = offp2;
      }

      if (closed) {
        {
          auto& p1 = ps[sz - 2];
          auto& p2 = ps[sz - 1];
          auto& p3 = ps[0];

          vtx_buff.push_back(
            {.pos = _offset_intersec(
               line_offset(p1, p2, -d),  //
               line_offset(p2, p3, -d)), //
             .uv  = {},
             .col = u8col(col)});
          vtx_buff.push_back(
            {.pos = _offset_intersec(
               line_offset(p1, p2, +d),  //
               line_offset(p2, p3, +d)), //
             .uv  = {},
             .col = u8col(col)});
        }
        {
          auto& p1 = ps[sz - 1];
          auto& p2 = ps[0];
          auto& p3 = ps[1];
          {
            auto intersec = _offset_intersec(
              line_offset(p1, p2, -d),  //
              line_offset(p2, p3, -d)); //
            vtx_buff.push_back({.pos = intersec, .uv = {}, .col = u8col(col)});
            vtx_buff[vtx_off + 0].pos = intersec;
          }
          {
            auto intersec = _offset_intersec(
              line_offset(p1, p2, +d),  //
              line_offset(p2, p3, +d)); //
            vtx_buff.push_back({.pos = intersec, .uv = {}, .col = u8col(col)});
            vtx_buff[vtx_off + 1].pos = intersec;
          }
        }
      } else {
        vtx_buff.push_back(
          {.pos = line_offset(ps[sz - 2], ps[sz - 1], -d).second,
           .uv  = {},
           .col = u8col(col)});
        vtx_buff.push_back(
          {.pos = line_offset(ps[sz - 2], ps[sz - 1], +d).second,
           .uv  = {},
           .col = u8col(col)});
      }

      for (size_t i = 1; i < (closed ? sz + 1 : sz); ++i) {
        auto i1 = (i - 1) * 2;
        auto i2 = i * 2;
        idx_buff.push_back({(uint16_t)(i1 + 1)});
        idx_buff.push_back({(uint16_t)(i1)});
        idx_buff.push_back({(uint16_t)(i2 + 1)});
        idx_buff.push_back({(uint16_t)(i1)});
        idx_buff.push_back({(uint16_t)(i2)});
        idx_buff.push_back({(uint16_t)(i2 + 1)});
      }
    }

    void dl__fill_convex_polygon(
      draw_list& dl,
      const std::span<glm::vec2>& ps,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex)
    {
      auto sz = ps.size();

      if (sz < 3)
        return;

      auto& vtx_buff = dl.vtx_buffer;
      auto& idx_buff = dl.idx_buffer;
      auto& cmd_buff = dl.cmd_buffer;

      uint32_t vtx_off  = vtx_buff.size();
      uint32_t idx_off  = idx_buff.size();
      uint32_t vtx_size = sz;
      uint32_t idx_size = (sz - 2) * 3;

      cmd_buff.push_back(
        {.idx_count  = idx_size,
         .idx_offset = idx_off,
         .vtx_offset = vtx_off,
         .tex        = tex,
         .scissor    = clip_scissor(scissor)});

      reserve_increase(vtx_buff, vtx_size);
      reserve_increase(idx_buff, idx_size);

      for (size_t i = 0; i < vtx_size; ++i)
        vtx_buff.push_back({.pos = ps[i], .uv = {}, .col = u8col(col)});

      for (size_t i = 2; i < vtx_size; ++i) {
        idx_buff.push_back({(uint16_t)(0)});
        idx_buff.push_back({(uint16_t)(i - 1)});
        idx_buff.push_back({(uint16_t)(i)});
      }
    }

    void add_arc_points(
      std::vector<glm::vec2>& ps,
      glm::vec2 center,
      float radius,
      float theta0,
      float theta1)
    {
      const auto pi   = std::numbers::pi_v<float>;
      const auto step = 1 / radius;
      const auto x    = center.x + 0.5f;
      const auto y    = center.y + 0.5f;

      theta0 -= pi / 2;
      theta1 -= pi / 2;

      reserve_increase(ps, (theta1 - theta0) / step + 1);

      for (auto t = theta0; t < theta1; t += step)
        ps.push_back({x + radius * std::cos(t), y + radius * std::sin(t)});
    }

    void add_rounded_rect_points(
      std::vector<glm::vec2>& ps,
      glm::vec2 p1,
      glm::vec2 p2,
      float r)
    {
      p1 += 0.5f;
      p2 -= 0.5f;
      auto pi = std::numbers::pi_v<float>;
      add_arc_points(ps, {p2.x - r, p1.y + r}, r, 0, pi / 2);
      add_arc_points(ps, {p2.x - r, p2.y - r}, r, pi / 2, pi);
      add_arc_points(ps, {p1.x + r, p2.y - r}, r, pi, 3 * pi / 2);
      add_arc_points(ps, {p1.x + r, p1.y + r}, r, 3 * pi / 2, 2 * pi);
    }
  } // namespace

  void dl_draw_line(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    std::array ps {p1 + 0.5f, p2 + 0.5f};
    dl__draw_polyline(dl, ps, width, polyline_flags::none, col, scissor, tex);
  }

  void dl_draw_polyline(
    draw_list& dl,
    const std::span<glm::vec2>& ps,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    dl__draw_polyline(dl, ps, width, polyline_flags::none, col, scissor, tex);
  }

  void dl_draw_closed_polyline(
    draw_list& dl,
    const std::span<glm::vec2>& ps,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    dl__draw_polyline(dl, ps, width, polyline_flags::closed, col, scissor, tex);
  }

  void dl_fill_convex_polygon(
    draw_list& dl,
    const std::span<glm::vec2>& ps,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    dl__fill_convex_polygon(dl, ps, col, scissor, tex);
  }

  void dl_draw_rect(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    auto q1 = p1 + 0.5f;
    auto q2 = p2 - 0.5f;
    auto ps = std::array {q1, glm::vec2(q2.x, q1.y), q2, glm::vec2(q1.x, q2.y)};
    dl__draw_polyline(dl, ps, width, polyline_flags::closed, col, scissor, tex);
  }

  void dl_fill_rect(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    auto q1 = p1 + 0.5f;
    auto q2 = p2 - 0.5f;
    auto ps = std::array {q1, glm::vec2(q2.x, q1.y), q2, glm::vec2(q1.x, q2.y)};
    dl__fill_convex_polygon(dl, ps, col, scissor, tex);
  }

  void dl_draw_rounded_rect(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const float& width,
    const float& radius,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    std::vector<glm::vec2> ps;
    add_rounded_rect_points(ps, p1, p2, radius);
    dl__draw_polyline(dl, ps, width, polyline_flags::closed, col, scissor, tex);
  }

  void dl_fill_rounded_rect(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const float& radius,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    std::vector<glm::vec2> ps;
    add_rounded_rect_points(ps, p1, p2, radius);
    dl__fill_convex_polygon(dl, ps, col, scissor, tex);
  }

  void dl_draw_circle(
    draw_list& dl,
    const glm::vec2& center,
    const float& radius,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    std::vector<glm::vec2> ps;
    add_arc_points(ps, center, radius, 0, 2 * std::numbers::pi_v<float>);
    dl__draw_polyline(dl, ps, width, polyline_flags::closed, col, scissor, tex);
  }

  void dl_fill_circle(
    draw_list& dl,
    const glm::vec2& center,
    const float& radius,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex)
  {
    std::vector<glm::vec2> ps;
    add_arc_points(ps, center, radius, 0, 2 * std::numbers::pi_v<float>);
    dl__fill_convex_polygon(dl, ps, col, scissor, tex);
  }
} // namespace yave::wm