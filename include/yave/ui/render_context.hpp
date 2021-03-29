//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/passkey.hpp>
#include <yave/ui/draw_list.hpp>
#include <yave/ui/vulkan_device.hpp>

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
    // Vulkan logical device
    ui::vulkan_device m_device;

  private:
    vk::UniqueSampler m_image_sampler;
    vk::UniqueDescriptorSetLayout m_descriptor_set_layout;
    vk::UniqueDescriptorPool m_descriptor_pool;
    vk::UniquePipelineCache m_pipeline_cache;
    vk::UniquePipelineLayout m_pipeline_layout;

    bool do_render_required(const window*);
    void do_render_viewport(viewport*);

  public:
    render_context(main_context& mctx);

    ~render_context() noexcept;

    // get vulkan device
    auto& vulkan_device()
    {
      return m_device;
    }

  public:
    /// Process render stage
    void do_render(window_manager& wm, passkey<view_context>);
    /// Render window
    auto render_window(const window* w, draw_list dl) -> draw_list;
    /// Setup viewport window
    void init_viewport(viewport* vp, passkey<viewport>);

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