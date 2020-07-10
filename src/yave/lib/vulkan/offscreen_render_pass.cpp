//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/offscreen_render_pass.hpp>
#include <yave/lib/vulkan/offscreen.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(composition_pass)

namespace {

  using namespace yave::vulkan;

  auto getRenderPassColorAttachment(const vk::Format& format)
  {
    vk::AttachmentDescription colorAttachment;

    // format
    colorAttachment.format = format;
    // sample count
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    // load from existing image
    colorAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
    // store result to buffer
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    // layout
    colorAttachment.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.finalLayout   = vk::ImageLayout::eColorAttachmentOptimal;

    return std::array {colorAttachment};
  }

  auto getRenderPassAttachmentReference()
  {
    vk::AttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;
    return std::array {colorAttachmentRef};
  }

  auto getSubpasses(
    const std::array<vk::AttachmentReference, 1>& colorAttachmentRefs)
  {
    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = (uint32_t)colorAttachmentRefs.size();
    subpass.pColorAttachments    = colorAttachmentRefs.data();

    return std::array {subpass};
  }

  auto getSubpassDependencies()
  {
    // initial subpass dependency
    vk::SubpassDependency dep;
    dep.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dep.dstSubpass    = 0;
    dep.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dep.srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead
                        | vk::AccessFlagBits::eColorAttachmentWrite;
    dep.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead
                        | vk::AccessFlagBits::eColorAttachmentWrite;

    return std::array {dep};
  }

  auto createRenderPass(const vk::Format& format, const vk::Device& device)
  {
    vk::RenderPassCreateInfo info;

    auto attachments     = getRenderPassColorAttachment(format);
    info.attachmentCount = (uint32_t)attachments.size();
    info.pAttachments    = attachments.data();

    auto attachmentRefs = getRenderPassAttachmentReference();
    auto subpasses      = getSubpasses(attachmentRefs);
    info.subpassCount   = (uint32_t)subpasses.size();
    info.pSubpasses     = subpasses.data();

    auto dependencies    = getSubpassDependencies();
    info.dependencyCount = (uint32_t)dependencies.size();
    info.pDependencies   = dependencies.data();

    return device.createRenderPassUnique(info);
  }

  auto createCommandPool(uint32_t graphicsQueueIndex, const vk::Device& device)
  {
    vk::CommandPoolCreateInfo info;
    // allow vkResetCommandBuffer
    info.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    info.queueFamilyIndex = graphicsQueueIndex;

    return device.createCommandPoolUnique(info);
  }

  auto createCommandBuffers(
    uint32_t size,
    const vk::CommandBufferLevel& level,
    const vk::CommandPool& commandPool,
    const vk::Device& device) -> std::vector<vk::UniqueCommandBuffer>
  {
    vk::CommandBufferAllocateInfo info {};
    info.commandPool        = commandPool;
    info.level              = level;
    info.commandBufferCount = size;

    return device.allocateCommandBuffersUnique(info);
  }

  auto createFence(const vk::FenceCreateFlags& flags, const vk::Device& device)
    -> vk::UniqueFence
  {
    vk::FenceCreateInfo info;
    info.flags = flags;
    return device.createFenceUnique(info);
  }

} // namespace

namespace yave::vulkan {

  class rgba32f_offscreen_render_pass::impl
  {
  public:
    offscreen_context& offscreen_ctx;
    vulkan_context& vulkan_ctx;

  public:
    vk::UniqueCommandPool command_pool;
    vk::UniqueCommandBuffer command_buffer;

  public: /* pipeline */
    vk::UniqueRenderPass render_pass;
    offscreen_frame_data frame_data;

  public:
    // fence to wait submitted queue operations finish
    vk::UniqueFence submit_fence;

  public:
    staging_buffer texture_staging;

  public:
    impl(uint32_t width, uint32_t height, offscreen_context& ctx)
      : offscreen_ctx {ctx}
      , vulkan_ctx {ctx.vulkan_ctx()}
    {
      init_logger();

      auto extent = vk::Extent2D {width, height};
      auto format = convert_to_format(image_format::rgba32f);

      auto device             = offscreen_ctx.device();
      auto graphicsQueue      = offscreen_ctx.graphics_queue();
      auto graphicsQueueIndex = offscreen_ctx.graphics_queue_index();
      auto physicalDevice     = vulkan_ctx.physical_device();

      // create render pass
      render_pass = createRenderPass(format, ctx.device());

      // create command pool for graphics
      command_pool = createCommandPool(graphicsQueueIndex, device);

      // command buffer for rendering
      command_buffer = std::move(createCommandBuffers(
        1, vk::CommandBufferLevel::ePrimary, command_pool.get(), device)[0]);

      // create fence
      submit_fence = createFence(vk::FenceCreateFlagBits::eSignaled, device);

      frame_data = create_offscreen_frame_data(
        extent,
        format,
        graphicsQueue,
        command_pool.get(),
        render_pass.get(),
        device,
        physicalDevice);

      clear_offscreen_frame_data(
        frame_data,
        vk::ClearColorValue(),
        graphicsQueue,
        command_pool.get(),
        device,
        physicalDevice);

      texture_staging = create_staging_buffer(1, device, physicalDevice);
    }

    ~impl() noexcept
    {
      offscreen_ctx.device().waitIdle();
    }

    void store_frame(
      const vk::Offset2D& offset,
      const vk::Extent2D& extent,
      const uint8_t* data)
    {
      store_offscreen_frame_data(
        texture_staging,
        frame_data,
        offset,
        extent,
        data,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());
    }

    void store_frame(
      const texture_data& src,
      const vk::Offset2D& srcOffset,
      const vk::Offset2D& offset,
      const vk::Extent2D& extent)
    {
      store_offscreen_frame_data(
        src,
        srcOffset,
        frame_data,
        offset,
        extent,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());
    }

    void load_frame(
      const vk::Offset2D& offset,
      const vk::Extent2D& extent,
      uint8_t* data)
    {
      load_offscreen_frame_data(
        texture_staging,
        frame_data,
        offset,
        extent,
        data,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());
    }

    void load_frame(
      texture_data& dst,
      const vk::Offset2D& dstOffset,
      const vk::Offset2D& offset,
      const vk::Extent2D& extent) 
    {
      load_offscreen_frame_data(
        frame_data,
        dstOffset,
        dst,
        offset,
        extent,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());
    }

    void clear_frame(const vk::ClearColorValue& col)
    {
      clear_offscreen_frame_data(
        frame_data,
        col,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());
    }

    auto create_texture(vk::Extent2D extent, vk::Format format) -> texture_data
    {
      auto tex = create_texture_data(
        extent.width,
        extent.height,
        format,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        vulkan_ctx.physical_device());

      return tex;
    }

    void write_texture(
      vulkan::texture_data& tex,
      const vk::Offset2D& offset,
      const vk::Extent2D& extent,
      const uint8_t* data)
    {
      vulkan::store_texture_data(
        texture_staging,
        tex,
        offset,
        extent,
        data,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        vulkan_ctx.physical_device());
    }

    void clear_texture(texture_data& tex, const vk::ClearColorValue& col)
    {
      auto physicalDevice = vulkan_ctx.physical_device();
      auto device         = offscreen_ctx.device();
      auto graphicsQueue  = offscreen_ctx.graphics_queue();

      vulkan::clear_texture_data(
        tex, col, graphicsQueue, command_pool.get(), device, physicalDevice);
    }

    auto begin_pass() -> vk::CommandBuffer
    {
      auto buffer = command_buffer.get();

      // begin command buffer
      {
        vk::CommandBufferBeginInfo info;
        info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        buffer.begin(info);
      }

      // begin render pass
      {
        vk::RenderPassBeginInfo info;
        info.renderPass        = render_pass.get();
        info.renderArea.extent = frame_data.extent;
        info.framebuffer       = frame_data.buffer.get();

        buffer.beginRenderPass(info, vk::SubpassContents::eInline);
      }

      return buffer;
    }

    void end_pass()
    {
      auto buffer = command_buffer.get();

      // end recording
      {
        buffer.endRenderPass();
        buffer.end();
      }

      // submit
      {
        vk::SubmitInfo info;
        info.commandBufferCount = 1;
        info.pCommandBuffers    = &command_buffer.get();

        // clear before submit
        offscreen_ctx.device().resetFences(submit_fence.get());

        auto err =
          offscreen_ctx.graphics_queue().submit(1, &info, submit_fence.get());

        if (err != vk::Result::eSuccess)
          throw std::runtime_error(
            "Failed to submit command buffer: " + vk::to_string(err));
      }
    }

    void wait_draw()
    {
      offscreen_ctx.device().waitForFences(
        submit_fence.get(), true, std::numeric_limits<uint64_t>::max());
    }
  };

  rgba32f_offscreen_render_pass::rgba32f_offscreen_render_pass(
    uint32_t width,
    uint32_t height,
    offscreen_context& ctx)
    : m_pimpl {std::make_unique<impl>(width, height, ctx)}
  {
  }

  rgba32f_offscreen_render_pass::~rgba32f_offscreen_render_pass() noexcept
  {
  }

  auto rgba32f_offscreen_render_pass::offscreen_ctx() -> offscreen_context&
  {
    return m_pimpl->offscreen_ctx;
  }

  auto rgba32f_offscreen_render_pass::width() const noexcept -> uint32_t
  {
    return m_pimpl->frame_data.extent.width;
  }

  auto rgba32f_offscreen_render_pass::height() const noexcept -> uint32_t
  {
    return m_pimpl->frame_data.extent.height;
  }

  auto rgba32f_offscreen_render_pass::format() const noexcept -> image_format
  {
    return image_format::rgba32f;
  }

  void rgba32f_offscreen_render_pass::store_frame(
    const vk::Offset2D& offset,
    const vk::Extent2D& extent,
    const uint8_t* data)
  {
    m_pimpl->store_frame(offset, extent, data);
  }

  void rgba32f_offscreen_render_pass::store_frame(
    const texture_data& src,
    const vk::Offset2D& srcOffset,
    const vk::Offset2D& offset,
    const vk::Extent2D& extent)
  {
    m_pimpl->store_frame(src, srcOffset, offset, extent);
  }

  void rgba32f_offscreen_render_pass::load_frame(
    const vk::Offset2D& offset,
    const vk::Extent2D& extent,
    uint8_t* data) const
  {
    m_pimpl->load_frame(offset, extent, data);
  }

  void rgba32f_offscreen_render_pass::load_frame(
    texture_data& dst,
    const vk::Offset2D& dstOffset,
    const vk::Offset2D& offset,
    const vk::Extent2D& extent) const
  {
    m_pimpl->load_frame(dst, dstOffset, offset, extent);
  }

  void rgba32f_offscreen_render_pass::clear_frame(
    const vk::ClearColorValue& col)
  {
    m_pimpl->clear_frame(col);
  }

  auto rgba32f_offscreen_render_pass::create_texture(
    const vk::Extent2D& extent,
    const vk::Format& format) -> vulkan::texture_data
  {
    return m_pimpl->create_texture(extent, format);
  }

  void rgba32f_offscreen_render_pass::write_texture(
    vulkan::texture_data& tex,
    const vk::Offset2D& offset,
    const vk::Extent2D& extent,
    const uint8_t* data)
  {
    m_pimpl->write_texture(tex, offset, extent, data);
  }

  void rgba32f_offscreen_render_pass::clear_texture(
    vulkan::texture_data& tex,
    const vk::ClearColorValue& color)
  {
    m_pimpl->clear_texture(tex, color);
  }

  auto rgba32f_offscreen_render_pass::frame_extent() const noexcept
    -> vk::Extent2D
  {
    return m_pimpl->frame_data.extent;
  }

  auto rgba32f_offscreen_render_pass::frame_image() const noexcept -> vk::Image
  {
    return m_pimpl->frame_data.image.get();
  }

  auto rgba32f_offscreen_render_pass::frame_image_view() const noexcept
    -> vk::ImageView
  {
    return m_pimpl->frame_data.view.get();
  }

  auto rgba32f_offscreen_render_pass::frame_format() const noexcept -> vk::Format
  {
    return m_pimpl->frame_data.format;
  }

  auto rgba32f_offscreen_render_pass::frame_memory() const noexcept -> vk::DeviceMemory
  {
    return m_pimpl->frame_data.memory.get();
  }

  auto rgba32f_offscreen_render_pass::frame_buffer() const noexcept -> vk::Framebuffer
  {
    return m_pimpl->frame_data.buffer.get();
  }

  auto rgba32f_offscreen_render_pass::command_pool() const noexcept -> vk::CommandPool
  {
    return m_pimpl->command_pool.get();
  }

  auto rgba32f_offscreen_render_pass::command_buffer() const noexcept -> vk::CommandBuffer
  {
    return m_pimpl->command_buffer.get();
  }

  auto rgba32f_offscreen_render_pass::render_pass() const noexcept -> vk::RenderPass
  {
    return m_pimpl->render_pass.get();
  }

  auto rgba32f_offscreen_render_pass::begin_pass() -> vk::CommandBuffer
  {
    return m_pimpl->begin_pass();
  }

  void rgba32f_offscreen_render_pass::end_pass()
  {
    m_pimpl->end_pass();
  }

  void rgba32f_offscreen_render_pass::wait_draw()
  {
    m_pimpl->wait_draw();
  }
} // namespace yave::vulkan