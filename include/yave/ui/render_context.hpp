//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/passkey.hpp>
#include <yave/ui/draw_list.hpp>
#include <yave/ui/vulkan_device.hpp>
#include <yave/ui/vulkan_allocator.hpp>
#include <yave/ui/texture.hpp>
#include <yave/ui/render_layer.hpp>

#include <cstdint>
#include <memory>

namespace yave::ui {

  class window;
  class window_manager;
  class view_context;
  class viewport;
  class viewport_renderer;
  class main_context;
  class vulkan_device;
  class layout_context;
  class render_scope;

  /// Per window render data
  struct window_render_data
  {
    // renderer for viewport, otherwise nullptr
    std::unique_ptr<viewport_renderer> renderer;

    window_render_data();
    ~window_render_data() noexcept;
  };

  class render_context
  {
    layout_context& m_lctx;

    ui::vulkan_device m_device;
    ui::vulkan_allocator m_allocator;

  private:
    vk::UniqueSampler m_image_sampler;
    vk::UniqueDescriptorSetLayout m_descriptor_set_layout;
    vk::UniqueDescriptorPool m_descriptor_pool;
    vk::UniquePipelineCache m_pipeline_cache;
    vk::UniquePipelineLayout m_pipeline_layout;

    std::unique_ptr<ui::texture> m_default_tex;
    vk::UniqueDescriptorSet m_default_tex_descriptor_set;

    bool do_render_required(const window*);
    void do_render_viewport(viewport*);

    auto do_render_child_window(
      const window* w,
      const render_scope& parent,
      render_layer&& rl) -> render_layer;

  public:
    render_context(main_context& mctx, layout_context& lctx);

    ~render_context() noexcept;

    // get vulkan device
    auto vulkan_device() -> vulkan_device&
    {
      return m_device;
    }

    // get vulkan allocator
    auto vulkan_allocator() -> vulkan_allocator&
    {
      return m_allocator;
    }

  public:
    /// Process render stage
    void do_render(window_manager& wm, passkey<view_context>);

    /// Render window
    /// \param w target window
    /// \param parent parent render scope
    /// \param layer layer to write
    auto render_child_window(
      const window* win,
      const render_scope& parent,
      render_layer layer,
      passkey<render_scope>) -> render_layer;

    /// Setup viewport window
    void init_viewport(viewport* vp, passkey<viewport>);

  public:
    /// Get default texture (1x1 white image)
    auto default_texture() const -> draw_tex;

  public:
    /// Convert draw_tex to descriptor set
    static auto draw_tex_to_descriptor_set(draw_tex tex) -> vk::DescriptorSet;
    /// Convert descriptor set to draw_tex
    static auto descriptor_set_to_draw_tex(vk::DescriptorSet set) -> draw_tex;

  public:
    auto image_sampler() const
    {
      return m_image_sampler.get();
    }

    auto descriptor_set_layout() const
    {
      return m_descriptor_set_layout.get();
    }

    auto descriptor_pool() const
    {
      return m_descriptor_pool.get();
    }

    auto pipeline_cache() const
    {
      return m_pipeline_cache.get();
    }

    auto pipeline_layout() const
    {
      return m_pipeline_layout.get();
    }
  };

} // namespace yave::ui