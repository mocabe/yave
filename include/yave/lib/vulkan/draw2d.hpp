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
#include <numeric>

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

  /// clip rect
  struct draw2d_clip
  {
    glm::vec2 p1;
    glm::vec2 p2;
  };

  /// Single draw call on vulkan
  struct draw2d_cmd
  {
    /// index count
    uint32_t idx_count;
    /// index offset
    uint32_t idx_offset;
    /// vertex offset
    uint32_t vtx_offset;
    /// texture
    draw2d_tex tex;
    /// clip rect
    draw2d_clip clip;
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

    auto total_vtx_count() const
    {
      return std::accumulate(
        draw_lists.begin(), draw_lists.end(), size_t(), [](auto acc, auto& dl) {
          return acc + dl.vtx_buffer.size();
        });
    }
    auto total_idx_count() const
    {
      return std::accumulate(
        draw_lists.begin(), draw_lists.end(), size_t(), [](auto acc, auto& dl) {
          return acc + dl.idx_buffer.size();
        });
    }
  };

} // namespace yave::vulkan
