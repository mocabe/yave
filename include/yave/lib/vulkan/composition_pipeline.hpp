//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/frame_buffer/frame_buffer.hpp>

#include <boost/gil.hpp>

namespace yave::vulkan {

  class rgba32f_composition_pipeline
  {
  public:
    using pixel_loc_type = boost::gil::rgba32f_loc_t;
    using pixel_type     = typename pixel_loc_type::value_type;

  public:
    rgba32f_composition_pipeline(
      uint32_t width,
      uint32_t height,
      vulkan_context& ctx);

    ~rgba32f_composition_pipeline() noexcept;

  public:
    /// Store frame onto existing image.
    /// Existing image will be overwritten by this operation.
    void store_frame(const boost::gil::rgba32fc_view_t& view);
    /// Load image back to image.
    void load_frame(const boost::gil::rgba32f_view_t& view) const;

  public:
    /// Start command buffer to record draw call
    auto begin_draw() -> vk::CommandBuffer;
    /// End command buffer and send to GPU
    void end_draw();

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };

} // namespace yave::vulkan