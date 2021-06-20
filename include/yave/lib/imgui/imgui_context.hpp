//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/vulkan/window_context.hpp>
#include <yave/lib/vulkan/texture.hpp>
#include <yave/core/enum_flag.hpp>

#include <imgui.h>
#include <imgui_freetype.h> // use freetype for font rendering

namespace yave::imgui {

  /// imgui application
  class imgui_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// init options
    struct create_info
    {
      /// initial width
      uint32_t width = 1280;
      /// initial height
      uint32_t height = 720;
      /// window name
      const char* name = "imgui_context";
    };

  private:
    static auto _init_create_info() -> create_info
    {
      return create_info();
    }

  public:
    /// Ctor
    imgui_context(
      vulkan::vulkan_context& vulkan_ctx,
      create_info info = _init_create_info());
    /// Dtor
    ~imgui_context() noexcept;
    /// no copy
    imgui_context(const imgui_context&) = delete;

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
    [[nodiscard]] auto glfw_context()   const -> const glfw::glfw_context&;
    [[nodiscard]] auto vulkan_context() const -> const vulkan::vulkan_context&;
    [[nodiscard]] auto window_context() const -> const vulkan::window_context&;
    [[nodiscard]] auto glfw_context()   -> glfw::glfw_context&;
    [[nodiscard]] auto vulkan_context() -> vulkan::vulkan_context&;
    [[nodiscard]] auto window_context() -> vulkan::window_context&;

  public:
    [[nodiscard]] auto font_sampler()          const -> vk::Sampler;
    [[nodiscard]] auto descriptor_pool()       const -> vk::DescriptorPool;
    [[nodiscard]] auto descriptor_set_layout() const -> vk::DescriptorSetLayout;
    [[nodiscard]] auto descriptor_set()        const -> vk::DescriptorSet;
    [[nodiscard]] auto pipeline_cache()        const -> vk::PipelineCache;
    [[nodiscard]] auto pipeline_layout()       const -> vk::PipelineLayout;
    [[nodiscard]] auto pipeline()              const -> vk::Pipeline;
    [[nodiscard]] auto font_image_memory()     const -> vk::DeviceMemory;
    [[nodiscard]] auto font_image()            const -> vk::Image;
    [[nodiscard]] auto font_image_view()       const -> vk::ImageView;
    // clang-format on

  public: /* texture utility */
    /// Add texture data
    [[nodiscard]] auto create_texture(
      const vk::Extent2D& extent,
      const vk::Format& format) -> vulkan::texture_data;

    /// Update texture data
    void write_texture(
      vulkan::texture_data& tex,
      vk::Offset2D offset,
      vk::Extent2D size,
      const uint8_t* srcData);

    /// Clear texture
    void clear_texture(
      vulkan::texture_data& tex,
      const vk::ClearColorValue& color);

  public: /* texture management */
    /// Bind texture to context.
    /// User should manually unbind textures.
    [[nodiscard]] auto bind_texture(const vulkan::texture_data& tex)
      -> ImTextureID;

    /// Unbind texture to context
    void unbind_texture(const vulkan::texture_data& tex);

  public: /* misc */
    /// Get clear color
    auto get_clear_color() const -> std::array<float, 4>;
    /// Set clear color
    void set_clear_color(float r, float g, float b, float a);
    /// Get FPS
    auto get_fps() const -> uint32_t;
    /// Set FPS
    void set_fps(uint32_t fps);
  };

} // namespace yave::imgui