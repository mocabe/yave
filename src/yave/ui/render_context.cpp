//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/render_context.hpp>
#include <yave/ui/render_scope.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/viewport.hpp>
#include <yave/ui/root.hpp>
#include <yave/ui/viewport_renderer.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/vulkan_device.hpp>
#include <yave/ui/vulkan_surface.hpp>

#include <ranges>
#include <optional>

namespace {

  using namespace yave::ui;

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
        .setMaxSets(1000 * static_cast<u32>(poolSizes.size()))
        .setPoolSizes(poolSizes);

    return device.createDescriptorPoolUnique(info);
  }

  auto createPipelineCache(const vk::Device& device)
  {
    auto info = vk::PipelineCacheCreateInfo();
    return device.createPipelineCacheUnique(info);
  }

  auto bindTexture(
    const vk::ImageView& tex,
    const vk::DescriptorPool& pool,
    const vk::DescriptorSetLayout& layout,
    const vk::DescriptorType& type,
    const vk::Device& device)
  {
    auto layouts = std::array {layout};

    auto dscInfo = vk::DescriptorSetAllocateInfo()
                     .setDescriptorPool(pool)
                     .setDescriptorSetCount(1)
                     .setSetLayouts(layouts);

    auto set = std::move(device.allocateDescriptorSetsUnique(dscInfo)[0]);

    auto imgInfo = vk::DescriptorImageInfo() //
                     .setImageView(tex)
                     .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    auto write = vk::WriteDescriptorSet()
                   .setDstSet(set.get())
                   .setDescriptorCount(1)
                   .setDescriptorType(type)
                   .setImageInfo(imgInfo);

    device.updateDescriptorSets(write, {});
    return set;
  }

} // namespace

namespace yave::ui {

  static_assert(sizeof(draw_tex) == sizeof(vk::DescriptorSet));

  auto render_context::draw_tex_to_descriptor_set(draw_tex tex)
    -> vk::DescriptorSet
  {
    // TODO: uset bit_cast
    vk::DescriptorSet set;
    std::memcpy(&set, &tex, sizeof(set));
    return set;
  }

  auto render_context::descriptor_set_to_draw_tex(vk::DescriptorSet set)
    -> draw_tex
  {
    // TODO: uset bit_cast
    draw_tex tex;
    std::memcpy(&tex, &set, sizeof(tex));
    return tex;
  }

  window_render_data::window_render_data()           = default;
  window_render_data::~window_render_data() noexcept = default;

  render_context::render_context(
    vulkan_context& vulkan,
    glfw_context& glfw,
    layout_context& lctx)
    : m_lctx {lctx}
    , m_device {vulkan, glfw}
    , m_allocator {m_device}
  {
    // clang-format off

    auto dev = m_device.device();

    // init resources
    m_image_sampler         = createImageSampler(dev);
    m_descriptor_set_layout = createDescriptorSetLayout(m_image_sampler.get(), dev);
    m_descriptor_pool       = createDescriptorPool(dev);
    m_pipeline_cache        = createPipelineCache(dev);

    // clang-format on

    m_default_tex = std::make_unique<ui::texture>(
      1, 1, vk::Format::eR8G8B8A8Unorm, m_allocator);

    m_default_tex->clear_color({1.f, 1.f, 1.f, 1.f});

    m_default_tex_descriptor_set = bindTexture(
      m_default_tex->image_view(),
      m_descriptor_pool.get(),
      m_descriptor_set_layout.get(),
      vk::DescriptorType::eCombinedImageSampler,
      m_device.device());
  }

  render_context::~render_context() noexcept = default;

  void render_context::do_render_viewport(viewport& vp)
  {
    // draw layers
    auto result = std::optional<ui::render_layer>();
    vp.render(render_scope(*this, m_lctx, vp, result));
    assert(result.has_value());
    // render
    auto& vctx = vp.window_manager().view_ctx();
    assert(vp.window_render_data({}).renderer);
    vp.window_render_data({}).renderer->render(std::move(*result), vctx);
  }

  bool render_context::do_render_ready(const viewport& vp)
  {
    return vp.window_render_data({}).renderer->can_render();
  }

  bool render_context::do_render_required(const window& w)
  {
    if (w.is_invalidated())
      return true;

    return std::ranges::any_of(
      w.children(), [&](auto&& c) { return do_render_required(c); });
  }

  void render_context::do_render(window_manager& wm, passkey<view_context>)
  {
    auto& root = wm.root();

    // re-render invalidated viewports
    for (auto&& vp : root.viewports()) {
      if (do_render_ready(vp) && do_render_required(vp)) {
        do_render_viewport(vp);
      }
    }
  }

  auto render_context::do_render_child_window(
    const window& w,
    const render_scope& parent,
    render_layer&& rl) -> render_layer
  {
    auto result = std::optional<ui::render_layer>();
    w.render(render_scope(*this, m_lctx, w, parent, std::move(rl), result));
    assert(result.has_value());
    return std::move(*result);
  }

  auto render_context::render_child_window(
    const window& win,
    const render_scope& parent,
    render_layer layer,
    passkey<render_scope>) -> render_layer
  {
    return do_render_child_window(win, parent, std::move(layer));
  }

  void render_context::init_viewport(viewport& vp, passkey<viewport>)
  {
    auto& data = vp.window_render_data({});
    // init renderer
    data.renderer = std::make_unique<viewport_renderer>(vp, *this);
  }

  auto render_context::default_texture() const -> draw_tex
  {
    return descriptor_set_to_draw_tex(m_default_tex_descriptor_set.get());
  }

} // namespace yave::ui