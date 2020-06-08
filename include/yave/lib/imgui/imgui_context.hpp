//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/vulkan/window_context.hpp>
#include <yave/support/enum_flag.hpp>

#include <imgui.h>
#include <imgui_freetype.h> // use freetype for font rendering

namespace yave::imgui {

  /// imgui application
  class imgui_context
  {
  public:
    enum class init_flags : uint32_t
    {
      enable_logging = 1 << 0,
    };

  private:
    static auto _init_flags() noexcept -> init_flags;

  public:
    /// Ctor
    imgui_context(
      vulkan::vulkan_context& vulkan_ctx,
      init_flags flags = _init_flags());
    /// Dtor
    ~imgui_context() noexcept;

  public:
    /// Set current context
    void set_current();
    /// Begin ImGui frame
    void begin_frame();
    /// End ImGui frame
    void end_frame();
    /// Render ImGui frame with Vulkan
    void render();

  public:
    /// underlying type of ImTextureID
    using texture_type = const vk::DescriptorSet*;

    /// get texture ID from image descriptor
    [[nodiscard]] auto get_texture_id(const vk::DescriptorSet& tex) const
      -> ImTextureID;

    // clang-format off
  public:
    [[nodiscard]] auto glfw_context() const   -> const glfw::glfw_context&;
    [[nodiscard]] auto glfw_context()         -> glfw::glfw_context&;
    [[nodiscard]] auto vulkan_context() const -> const vulkan::vulkan_context&;
    [[nodiscard]] auto vulkan_context()       -> vulkan::vulkan_context&;
    [[nodiscard]] auto window_context() const -> const vulkan::window_context&;
    [[nodiscard]] auto window_context()       ->  vulkan::window_context&;
    // clang-format on

  public:
    [[nodiscard]] auto font_sampler() const -> vk::Sampler;
    [[nodiscard]] auto descriptor_pool() const -> vk::DescriptorPool;
    [[nodiscard]] auto descriptor_set_layout() const -> vk::DescriptorSetLayout;
    [[nodiscard]] auto descriptor_set() const -> vk::DescriptorSet;
    [[nodiscard]] auto pipeline_cache() const -> vk::PipelineCache;
    [[nodiscard]] auto pipeline_layout() const -> vk::PipelineLayout;
    [[nodiscard]] auto pipeline() const -> vk::Pipeline;
    [[nodiscard]] auto font_image_memory() const -> vk::DeviceMemory;
    [[nodiscard]] auto font_image() const -> vk::Image;
    [[nodiscard]] auto font_image_view() const -> vk::ImageView;

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

    /// Update texture data
    void update_texture(
      const std::string&,
      const uint8_t* srcData,
      const vk::DeviceSize& srcSize);

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

} // namespace yave::imgui

namespace yave {

  // enable flags
  template <>
  struct is_enum_flag<imgui::imgui_context::init_flags> : std::true_type
  {
  };

} // namespace yave