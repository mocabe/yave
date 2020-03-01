//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/vulkan/draw_flags.hpp>
#include <yave/support/enum_flag.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace yave::vulkan {

  /// Draw index
  struct draw2d_idx
  {
    /// index
    uint16_t idx;
  };

  /// Draw vertex
  struct draw2d_vtx
  {
    /// position of vertex
    glm::vec2 pos;
    /// uv
    glm::vec2 uv;
    /// color
    glm::vec4 col;
  };

  /// Draw texture
  struct draw2d_tex
  {
    /// descriptor set
    vk::DescriptorSet descriptor_set;
  };

  /// push constants
  struct draw2d_pc
  {
    // transformation to NDC
    glm::mat3 transform;
  };

  /// Single draw call on vulkan
  struct draw2d_cmd
  {
    /// clip rect
    glm::vec4 clip;
    /// index count
    uint32_t idx_count;
    /// vertex count
    uint32_t vtx_count;
    /// index offset
    uint32_t idx_offset;
    /// vertex offset
    uint32_t vtx_offset;
    /// texture
    draw2d_tex tex;
  };

  /// Set of draw commands
  struct draw2d_list
  {
    /// Index buffer
    std::vector<draw2d_idx> idx_buffer;
    /// Vertex buffer
    std::vector<draw2d_vtx> vtx_buffer;
    /// Commands
    std::vector<draw2d_cmd> cmd_buffer;
  };

  /// Draw data
  struct draw2d_data
  {
    /// set of draw list
    std::vector<draw2d_list> draw_lists;
    /// viewport to render
    vk::Viewport viewport;
  };

} // namespace yave::vulkan
