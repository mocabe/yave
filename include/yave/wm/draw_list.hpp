//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace yave::wm {

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
    glm::u8vec4 col;
  };

  /// texture
  struct draw_tex
  {
    /// texture handle
    uint64_t handle;
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
    uint32_t idx_count;
    /// index offset
    uint32_t idx_offset;
    /// vertex offset
    uint32_t vtx_offset;
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
  };

} // namespace yave::wm