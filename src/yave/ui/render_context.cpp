//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include "yave/lib/vulkan/texture.hpp"
#include <yave/ui/render_context.hpp>
#include <yave/ui/render_scope.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/viewport.hpp>
#include <yave/ui/root.hpp>
#include <yave/ui/viewport_renderer.hpp>
#include <yave/ui/main_context.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/vulkan_device.hpp>
#include <yave/ui/vulkan_surface.hpp>

namespace {

  auto createImageSampler(const vk::Device& device)
  {
    auto info = vk::SamplerCreateInfo()
                  .setMagFilter(vk::Filter::eLinear)
                  .setMinFilter(vk::Filter::eLinear)
                  .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                  .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeW(vk::SamplerAddressMode::eRepeat);

    return device.createSamplerUnique(info);
  }

  auto createDescriptorSetLayout(
    const vk::Sampler& sampler,
    const vk::Device& device)
  {
    std::array bind = {
      vk::DescriptorSetLayoutBinding()
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(1)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment)
        .setPImmutableSamplers(&sampler)};

    auto info = vk::DescriptorSetLayoutCreateInfo().setBindings(bind);

    return device.createDescriptorSetLayoutUnique(info);
  }

  auto createDescriptorPool(const vk::Device& device)
  {
    std::array poolSizes = {
      vk::DescriptorPoolSize {vk::DescriptorType::eCombinedImageSampler, 1000}};

    auto info =
      vk::DescriptorPoolCreateInfo()
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
        .setMaxSets(1000 * poolSizes.size())
        .setPoolSizes(poolSizes);

    return device.createDescriptorPoolUnique(info);
  }

  auto createPipelineLayout(
    const vk::DescriptorSetLayout& setLayout,
    const vk::Device& device)
  {
    std::array pcr = {vk::PushConstantRange()
                        .setStageFlags(vk::ShaderStageFlagBits::eVertex)
                        .setSize(sizeof(yave::ui::draw_pc))};

    std::array layout = {setLayout};

    auto info = vk::PipelineLayoutCreateInfo()
                  .setSetLayouts(layout)
                  .setPushConstantRanges(pcr);

    return device.createPipelineLayoutUnique(info);
  }

  auto createPipelineCache(const vk::Device& device)
  {
    auto info = vk::PipelineCacheCreateInfo();
    return device.createPipelineCacheUnique(info);
  }

} // namespace

namespace yave::ui {

  window_render_data::window_render_data()           = default;
  window_render_data::~window_render_data() noexcept = default;

  render_context::render_context(main_context& mctx)
    : m_device {mctx.vulkan_ctx(), mctx.glfw_ctx()}
  {
    // clang-format off

    auto dev = m_device.device();

    // init resources
    m_image_sampler         = createImageSampler(dev);
    m_descriptor_set_layout = createDescriptorSetLayout(m_image_sampler.get(), dev);
    m_descriptor_pool       = createDescriptorPool(dev);
    m_pipeline_cache        = createPipelineCache(dev);
    m_pipeline_layout       = createPipelineLayout(m_descriptor_set_layout.get(), dev);

    // clang-format on
  }

  render_context::~render_context() noexcept = default;

  void render_context::do_render_viewport(viewport* vp)
  {
    auto dl = render_window(vp, draw_list());
    assert(vp->get_window_render_data({})->renderer);
    vp->get_window_render_data({})->renderer->render(std::move(dl));
  }

  bool render_context::do_render_required(const window* w)
  {
    if (w->is_invalidated())
      return true;

    for (auto&& c : w->children()) {
      if (do_render_required(c))
        return true;
    }
    return false;
  }

  void render_context::do_render(window_manager& wm, passkey<view_context>)
  {
    auto root = wm.root();

    // re-render invalidated viewports
    for (auto&& vp : root->viewports())
      if (do_render_required(vp))
        do_render_viewport(vp);
  }

  auto render_context::render_window(const window* w, draw_list dl) -> draw_list
  {
    auto out_dl = std::optional<draw_list>();
    w->render(render_scope(*this, std::move(dl), out_dl));

    assert(out_dl.has_value());
    return std::move(out_dl.value());
  }

  void render_context::init_viewport(viewport* vp, passkey<viewport>)
  {
    assert(vp);
    auto& data = *vp->get_window_render_data({});
    // init renderer
    data.renderer = std::make_unique<viewport_renderer>(*vp, *this);
  }

} // namespace yave::ui