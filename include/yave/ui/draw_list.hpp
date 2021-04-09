//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/typedefs.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <span>

namespace yave::ui {

  /// push constants
  struct draw_pc
  {
    glm::vec2 translate;
    glm::vec2 scale;
  };

  /// draw command index
  struct draw_idx
  {
    /// index value
    u16 idx;
  };

  /// vertex
  struct draw_vtx
  {
    /// position of vertex
    glm::vec2 pos;
    /// uv
    glm::vec2 uv;
    /// color
    glm::u8vec4 col;
  };

  /// texture
  struct draw_tex
  {
    /// texture handle
    u64 handle;
  };

  /// scissor rect
  struct draw_scissor
  {
    /// upper left corner
    glm::vec2 offset;
    /// size of scissor rect
    glm::vec2 extent;
  };

  /// single draw call
  struct draw_cmd
  {
    /// index count
    u32 idx_count;
    /// index offset
    u32 idx_offset;
    /// vertex offset
    u32 vtx_offset;
    /// texture
    draw_tex tex;
    /// scissor rect
    draw_scissor scissor;
  };

  /// Set of draw commands
  struct draw_list
  {
    /// Index buffer
    std::vector<draw_idx> idx_buffer;
    /// Vertex buffer
    std::vector<draw_vtx> vtx_buffer;
    /// Commands
    std::vector<draw_cmd> cmd_buffer;

  public:
    /// add line
    void draw_line(
      const glm::vec2& p1,
      const glm::vec2& p2,
      const f32& width,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);

    /// add polyline
    void draw_polyline(
      const std::span<glm::vec2>& ps,
      const f32& width,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);

    /// add closed polyline
    void draw_closed_polyline(
      const std::span<glm::vec2>& ps,
      const f32& width,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);

    /// add convex polygon
    void fil_convex_polygon(
      const std::span<glm::vec2>& ps,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);

    /// add rect
    /// \param p1 upper left corner
    /// \param p2 lower right corner
    void draw_rect(
      const glm::vec2& p1,
      const glm::vec2& p2,
      const f32& width,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);

    /// add rect
    /// \param p1 upper left corner
    /// \param p2 lower right corner
    void fill_rect(
      const glm::vec2& p1,
      const glm::vec2& p2,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);

    /// add rounded rect
    /// \param p1 upper left corner
    /// \param p2 lower right corner
    void draw_rounded_rect(
      const glm::vec2& p1,
      const glm::vec2& p2,
      const f32& width,
      const f32& radius,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);

    /// add rounded rect
    /// \param p1 upper left corner
    /// \param p2 lower right corner
    void fill_rounded_rect(
      const glm::vec2& p1,
      const glm::vec2& p2,
      const f32& radius,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);

    /// add circle
    void draw_circle(
      const glm::vec2& center,
      const f32& radius,
      const f32& width,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);

    /// add circle
    void fill_circle(
      const glm::vec2& center,
      const f32& radius,
      const glm::vec4& col,
      const draw_scissor& scissor,
      const draw_tex& tex);
  };

  class draw_lists
  {
    /// set of draw list
    std::vector<draw_list> m_lists;

  public:
    draw_lists()                      = default;
    draw_lists(const draw_list&)      = delete;
    draw_lists(draw_lists&&) noexcept = default;

    auto lists() const
    {
      return std::span<const draw_list>(m_lists);
    }

    auto lists()
    {
      return std::span<draw_list>(m_lists);
    }

  public:
    /// calculate total vertex
    auto total_vtx_count() const
    {
      size_t sum = 0;
      for (auto dl : m_lists)
        sum += dl.vtx_buffer.size();
      return sum;
    }

    /// calculate total index
    auto total_idx_count() const
    {
      size_t sum = 0;
      for (auto dl : m_lists)
        sum += dl.idx_buffer.size();
      return sum;
    }
  };
} // namespace yave::ui