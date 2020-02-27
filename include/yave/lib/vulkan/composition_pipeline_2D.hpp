//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/composition_pass.hpp>

#include <glm/glm.hpp>

namespace yave::vulkan {

  /// Basic 2D pipeline using Vulkan
  class rgba32f_composition_pipeline_2D
  {
  public:
    /// vertex
    struct vert
    {
      glm::vec2 pos;
      glm::vec2 uv;
      glm::vec4 col;
    };
    /// push constants
    struct pc
    {
      // transformation to NDC
      glm::mat3 transform;
    };

  public:
    /// Ctor
    rgba32f_composition_pipeline_2D(
      uint32_t width,
      uint32_t height,
      vulkan_context& ctx);
    /// Dtor
    ~rgba32f_composition_pipeline_2D() noexcept;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
}