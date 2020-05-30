//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace yave::editor {

  // FIXME: These things should be shared with all 2D renedring engines ideally.

  /// index
  struct draw_idx
  {
    /// index value
    uint16_t idx;
  };

  /// vertex
  struct draw_vtx
  {
    /// position of vertex
    glm::vec2 pos;
    /// uv
    glm::vec2 uv;
    /// color
    glm::vec4 col;
  };

  /// texture
  struct draw_tex
  {
    /// texture handle
    uint64_t handle;
  };

  /// push constants
  struct draw_pc
  {
    glm::vec2 translate;
    glm::vec2 scale;
  };

  /// clip rect
  struct draw_clip
  {
    glm::vec2 p1;
    glm::vec2 p2;
  };

  /// draw call
  struct draw_cmd
  {
    /// index count
    uint32_t idx_count;
    /// index offset
    uint32_t idx_offset;
    /// vertex offset
    uint32_t vtx_offset;
    /// texture
    draw_tex tex;
    /// clip rect
    draw_clip clip;
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
  };

  /// Draw data
  struct draw_lists
  {
    /// set of draw list
    std::vector<draw_list> lists;

    /// calculate total vertex
    auto total_vtx_count() const
    {
      size_t sum = 0;
      for (auto dl : lists)
        sum += dl.vtx_buffer.size();
      return sum;
    }

    /// calculate total index
    auto total_idx_count() const
    {
      size_t sum = 0;
      for (auto dl : lists)
        sum += dl.idx_buffer.size();
      return sum;
    }
  };

} // namespace yave::editor
