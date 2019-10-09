//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>

#include <imgui.h>
#include <imgui_freetype.h> // use freetype for font rendering
#include <chrono>
#include <map>
#include <string>
#include <memory>

namespace yave::imgui {

  // clang-format off

  /// imgui application
  class imgui_context
  {
  public:
    /// Ctor
    imgui_context(bool enableValidation = true);
    /// Dtor
    ~imgui_context();
  
  public:
    /// Begin ImGui frame
    void begin();
    /// End ImGui frame
    void end();
    /// Render ImGui frame with Vulkan
    void render();

  public:
    /// underlying type of ImTextureID
    using texture_type = const vk::DescriptorSet*;

    /// get texture ID from image descriptor
    [[nodiscard]] auto get_texture_id(vk::DescriptorSet& tex) const -> ImTextureID;

    // clang-format off
  public:
    [[nodiscard]] auto glfw_context() const   -> const glfw::glfw_context&;
    [[nodiscard]] auto vulkan_context() const -> const vulkan::vulkan_context&;
    [[nodiscard]] auto window_context() const -> const vulkan::window_context&;
    // clang-format on

  public:
    [[nodiscard]] auto font_sampler() const          -> vk::Sampler;
    [[nodiscard]] auto descriptor_pool() const       -> vk::DescriptorPool;
    [[nodiscard]] auto descriptor_set_layout() const -> vk::DescriptorSetLayout;
    [[nodiscard]] auto descriptor_set() const        -> vk::DescriptorSet;
    [[nodiscard]] auto pipeline_cache() const        -> vk::PipelineCache;
    [[nodiscard]] auto pipeline_layout() const       -> vk::PipelineLayout;
    [[nodiscard]] auto pipeline() const              -> vk::Pipeline;
    [[nodiscard]] auto font_image_memory() const     -> vk::DeviceMemory;
    [[nodiscard]] auto font_image() const            -> vk::Image;
    [[nodiscard]] auto font_image_view() const       -> vk::ImageView;

  public: /* texture management */
    /// Add texture data
    [[nodiscard]] auto add_texture(
      const std::string& name,
      const vk::Extent2D& extent,
      const vk::DeviceSize& byte_size,
      const vk::Format& format,
      const uint8_t* data) -> ImTextureID;

    /// Find texture data from name
    [[nodiscard]] auto find_texture(const std::string& name) const
      -> ImTextureID;

    /// Remove texture data
    void remove_texture(const std::string& name);

  public: /* misc */
    /// Get clear color
    auto get_clear_color() const -> std::array<float, 4>;
    /// Set clear color
    void set_clear_color(float r, float g, float b, float a);
    /// Get FPS
    auto get_fps() const -> uint32_t;
    /// Set FPS
    void set_fps(uint32_t fps);

  private:
    imgui_context(const imgui_context&) = delete;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };

  // clang-format on
}