//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/composition_pass.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/lib/image/blend_operation.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(frame_compositor)

namespace {

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
    colorAttachment.initialLayout = vk::ImageLayout::eGeneral;
    colorAttachment.finalLayout   = vk::ImageLayout::eGeneral;

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

  auto findMemoryTypeIndex(
    const vk::MemoryRequirements& requirements,
    const vk::MemoryPropertyFlags& properties,
    const vk::PhysicalDevice& physicalDevice)
  {
    auto props = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < props.memoryTypeCount; ++i) {
      auto memType = props.memoryTypes[i];
      if (requirements.memoryTypeBits & (1 << i))
        if ((memType.propertyFlags & properties) == properties)
          return i;
    }
    throw std::runtime_error("Failed to find suitable memory type");
  }

  auto createImage(
    const vk::Extent2D& extent,
    const vk::Format& format,
    const vk::Device& device)
  {
    vk::ImageCreateInfo info;
    info.imageType   = vk::ImageType::e2D;
    info.format      = format;
    info.extent      = vk::Extent3D {extent, 1};
    info.mipLevels   = 1;
    info.arrayLayers = 1;
    info.samples     = vk::SampleCountFlagBits::e1;
    info.tiling      = vk::ImageTiling::eOptimal;
    info.usage = vk::ImageUsageFlagBits::eColorAttachment // as frame buffer
                 | vk::ImageUsageFlagBits::eTransferDst   // as copy dst
                 | vk::ImageUsageFlagBits::eTransferSrc;  // as copy src
    info.sharingMode   = vk::SharingMode::eExclusive;
    info.initialLayout = vk::ImageLayout::eUndefined;

    return device.createImageUnique(info);
  }

  auto createImageMemory(
    const vk::Image& image,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    auto req = device.getImageMemoryRequirements(image);

    yave::Info(g_logger, "req.size: {}", req.size);
    yave::Info(g_logger, "req.alignment: {}", req.alignment);

    // device local memory
    vk::MemoryAllocateInfo info;
    info.allocationSize  = req.size;
    info.memoryTypeIndex = findMemoryTypeIndex(
      req, vk::MemoryPropertyFlagBits::eDeviceLocal, physicalDevice);

    auto mem = device.allocateMemoryUnique(info);
    device.bindImageMemory(image, mem.get(), 0);
    return mem;
  }

  auto createImageSubResourceRange()
  {
    vk::ImageSubresourceRange subResourceRange;
    subResourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    subResourceRange.levelCount = 1;
    subResourceRange.layerCount = 1;

    return subResourceRange;
  }

  auto createImageView(
    const vk::Image& image,
    const vk::Format& format,
    const vk::ImageSubresourceRange& range,
    const vk::Device& device)
  {

    vk::ImageViewCreateInfo info;
    info.image            = image;
    info.viewType         = vk::ImageViewType::e2D;
    info.format           = format;
    info.subresourceRange = range;

    return device.createImageViewUnique(info);
  }

  auto clearImage(
    const vk::Image& image,
    const vk::ClearColorValue& color,
    const vk::ImageSubresourceRange& range,
    const vk::Device& device,
    const vk::Queue& commandQueue,
    const vk::CommandPool& commandPool)
  {
    using namespace yave::vulkan;
    auto stc = single_time_command(device, commandQueue, commandPool);
    auto cmd = stc.command_buffer();

    // eUndefined -> eGeneral
    {
      vk::ImageMemoryBarrier barrier;
      barrier.oldLayout        = vk::ImageLayout::eUndefined;
      barrier.newLayout        = vk::ImageLayout::eGeneral;
      barrier.image            = image;
      barrier.subresourceRange = range;
      barrier.dstAccessMask    = vk::AccessFlagBits::eTransferWrite;

      auto srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
      auto dstStage = vk::PipelineStageFlagBits::eTransfer;

      cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    }

    // clear image
    {
      std::array ranges {range};
      cmd.clearColorImage(image, vk::ImageLayout::eGeneral, color, ranges);
    }
  }

  auto createFrameBuffer(
    const vk::ImageView& imageView,
    const vk::RenderPass& renderPass,
    const vk::Extent2D& extent,
    const vk::Device& device)
  {
    vk::FramebufferCreateInfo info;
    info.renderPass      = renderPass;
    info.attachmentCount = 1;
    info.pAttachments    = &imageView;
    info.width           = extent.width;
    info.height          = extent.height;
    info.layers          = 1;

    return device.createFramebufferUnique(info);
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

  auto createSemaphore(const vk::Device& device)
  {
    vk::SemaphoreCreateInfo info;
    return device.createSemaphoreUnique(info);
  }

  auto createFence(const vk::FenceCreateFlags& flags, const vk::Device& device)
    -> vk::UniqueFence
  {
    vk::FenceCreateInfo info;
    info.flags = flags;
    return device.createFenceUnique(info);
  }

  void storeFrame(
    const std::byte* srcData,
    const uint32_t& srcSize,
    const vk::Image& dstImage,
    const vk::Extent2D& dstImageExtent,
    const vk::CommandPool& commandPool,
    const vk::Queue& commandQueue,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    // create staging buffer
    vk::UniqueBuffer buffer;
    {
      vk::BufferCreateInfo info;
      info.size        = srcSize;
      info.usage       = vk::BufferUsageFlagBits::eTransferSrc;
      info.sharingMode = vk::SharingMode::eExclusive;

      buffer = device.createBufferUnique(info);
    }

    // create memory for stating buffer
    vk::UniqueDeviceMemory bufferMemory;
    {
      auto memReq = device.getBufferMemoryRequirements(buffer.get());
      vk::MemoryAllocateInfo info;
      info.allocationSize = memReq.size;
      // should be host visible, coherent
      info.memoryTypeIndex = findMemoryTypeIndex(
        memReq,
        vk::MemoryPropertyFlagBits::eHostVisible
          | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice);

      bufferMemory = device.allocateMemoryUnique(info);
      device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
    }

    /* upload data */
    {
      void* ptr = device.mapMemory(bufferMemory.get(), 0, srcSize);
      std::memcpy(ptr, srcData, srcSize);
      device.unmapMemory(bufferMemory.get());
    }

    using namespace yave::vulkan;
    auto stc = single_time_command(device, commandQueue, commandPool);
    auto cmd = stc.command_buffer();

    /* copy: buffer -> image */
    {
      vk::BufferImageCopy region;
      region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      region.imageSubresource.layerCount = 1;
      region.imageExtent                 = vk::Extent3D {dstImageExtent, 1};

      cmd.copyBufferToImage(
        buffer.get(), dstImage, vk::ImageLayout::eGeneral, region);
    }
  }

  void loadFrame(
    std::byte* dstData,
    uint32_t dstSize,
    const vk::Image& srcImage,
    const vk::Extent2D& srcImageExtent,
    const vk::CommandPool& commandPool,
    const vk::Queue& commandQueue,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    // staging buffer
    vk::UniqueBuffer buffer;
    {
      vk::BufferCreateInfo info;
      info.size        = dstSize;
      info.usage       = vk::BufferUsageFlagBits::eTransferDst;
      info.sharingMode = vk::SharingMode::eExclusive;

      buffer = device.createBufferUnique(info);
    }

    vk::UniqueDeviceMemory bufferMemory;
    {
      auto memReq = device.getBufferMemoryRequirements(buffer.get());

      vk::MemoryAllocateInfo info;
      info.allocationSize  = memReq.size;
      info.memoryTypeIndex = findMemoryTypeIndex(
        memReq,
        vk::MemoryPropertyFlagBits::eHostVisible
          | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice);

      bufferMemory = device.allocateMemoryUnique(info);
      device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
    }

    // image -> buffer
    {
      using namespace yave::vulkan;
      auto stc = single_time_command(device, commandQueue, commandPool);
      auto cmd = stc.command_buffer();

      vk::BufferImageCopy region;
      region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      region.imageSubresource.layerCount = 1;
      region.imageExtent                 = vk::Extent3D {srcImageExtent, 1};

      cmd.copyImageToBuffer(
        srcImage, vk::ImageLayout::eGeneral, buffer.get(), region);
    }

    // buffer -> host memory
    {
      const void* ptr = device.mapMemory(bufferMemory.get(), 0, dstSize);
      std::memcpy(dstData, ptr, dstSize);
      device.unmapMemory(bufferMemory.get());
    }
  }
} // namespace

namespace yave::vulkan {

  inline auto gilToVkFormat(boost::gil::rgba32f_pixel_t)
  {
    return vk::Format::eR32G32B32A32Sfloat;
  }

  template <class PixelLocType>
  struct composition_pass_impl
  {
  public:
    using pixel_loc_type = PixelLocType;
    using pixel_type     = typename pixel_loc_type::value_type;

  public:
    vulkan_context& context;

  public:
    vk::Format image_format;
    vk::Extent2D image_extent;

  public: /* pipeline */
    vk::UniqueRenderPass render_pass;
    vk::UniqueImage image;
    vk::UniqueDeviceMemory image_memory;
    vk::ImageSubresourceRange image_range;
    vk::UniqueImageView image_view;
    vk::UniqueFramebuffer frame_buffer;

  public:
    vk::UniqueCommandPool command_pool;
    vk::UniqueCommandBuffer command_buffer;

  public:
    // fence to wait submitted queue operations finish
    vk::UniqueFence submit_fence;

  public:
    composition_pass_impl(uint32_t width, uint32_t height, vulkan_context& ctx)
      : context {ctx}
    {
      init_logger();

      image_format = gilToVkFormat(pixel_type());
      image_extent = vk::Extent2D {width, height};

      // create render pass
      render_pass = createRenderPass(image_format, ctx.device());

      // create image for color attachment
      image = createImage(image_extent, image_format, ctx.device());

      // bind memory
      image_memory =
        createImageMemory(image.get(), ctx.device(), ctx.physical_device());

      // sub resourece range for image
      image_range = createImageSubResourceRange();

      // create view
      image_view =
        createImageView(image.get(), image_format, image_range, ctx.device());

      // create frame buffer
      frame_buffer = createFrameBuffer(
        image_view.get(), render_pass.get(), image_extent, ctx.device());

      // create command pool for graphics
      command_pool =
        createCommandPool(ctx.graphics_queue_family_index(), ctx.device());

      // command buffer for rendering
      command_buffer = std::move(createCommandBuffers(
        1,
        vk::CommandBufferLevel::ePrimary,
        command_pool.get(),
        context.device())[0]);

      // create fence
      submit_fence =
        createFence(vk::FenceCreateFlagBits::eSignaled, ctx.device());

      // clear image
      clearImage(
        image.get(),
        vk::ClearColorValue(),
        image_range,
        ctx.device(),
        ctx.graphics_queue(),
        command_pool.get());
    }

    ~composition_pass_impl() noexcept
    {
      context.device().waitIdle();
    }

    void store_frame(
      const boost::gil::image_view<typename pixel_loc_type::const_t>& view)
    {
      if (
        image_extent
        != vk::Extent2D {static_cast<uint32_t>(view.width()),
                         static_cast<uint32_t>(view.height())})
        throw std::runtime_error("Incompatible size for frame buffer");

      assert(view.is_1d_traversable());

      wait_draw();

      storeFrame(
        reinterpret_cast<const std::byte*>(view.row_begin(0)),
        static_cast<uint32_t>(view.size() * sizeof(pixel_type)),
        image.get(),
        image_extent,
        command_pool.get(),
        context.graphics_queue(),
        context.device(),
        context.physical_device());
    }

    void load_frame(const boost::gil::image_view<pixel_loc_type>& view)
    {
      if (
        image_extent
        != vk::Extent2D {static_cast<uint32_t>(view.width()),
                         static_cast<uint32_t>(view.height())})
        throw std::runtime_error("Incompatible size for frame buffer");

      assert(view.is_1d_traversable());

      wait_draw();

      loadFrame(
        reinterpret_cast<std::byte*>(view.row_begin(0)),
        static_cast<uint32_t>(view.size() * sizeof(pixel_type)),
        image.get(),
        image_extent,
        command_pool.get(),
        context.graphics_queue(),
        context.device(),
        context.physical_device());
    }

    auto begin_draw() -> vk::CommandBuffer
    {
      wait_draw();

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
        info.renderArea.extent = image_extent;
        info.framebuffer       = frame_buffer.get();

        buffer.beginRenderPass(info, vk::SubpassContents::eInline);
      }

      return buffer;
    }

    void end_draw()
    {
      wait_draw();

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
        context.device().resetFences(submit_fence.get());

        auto err =
          context.graphics_queue().submit(1, &info, submit_fence.get());

        if (err != vk::Result::eSuccess)
          throw std::runtime_error(
            "Failed to submit command buffer: " + vk::to_string(err));
      }
    }

    void wait_draw()
    {
      context.device().waitForFences(
        submit_fence.get(), true, std::numeric_limits<uint64_t>::max());
    }
  };

  class rgba32f_composition_pass::impl
    : public composition_pass_impl<pixel_loc_type>
  {
    using composition_pass_impl::composition_pass_impl;
  };

  rgba32f_composition_pass::rgba32f_composition_pass(
    uint32_t width,
    uint32_t height,
    vulkan_context& ctx)
    : m_pimpl {std::make_unique<impl>(width, height, ctx)}
  {
  }

  rgba32f_composition_pass::~rgba32f_composition_pass() noexcept
  {
  }

  void rgba32f_composition_pass::store_frame(
    const boost::gil::rgba32fc_view_t& view)
  {
    m_pimpl->store_frame(view);
  }

  void rgba32f_composition_pass::load_frame(
    const boost::gil::rgba32f_view_t& view) const
  {
    m_pimpl->load_frame(view);
  }

  auto rgba32f_composition_pass::frame_extent() const -> vk::Extent2D
  {
    return m_pimpl->image_extent;
  }

  auto rgba32f_composition_pass::frame_image() const -> vk::Image
  {
    return m_pimpl->image.get();
  }

  auto rgba32f_composition_pass::frame_image_view() const -> vk::ImageView
  {
    return m_pimpl->image_view.get();
  }

  auto rgba32f_composition_pass::frame_format() const -> vk::Format
  {
    return m_pimpl->image_format;
  }

  auto rgba32f_composition_pass::frame_memory() const -> vk::DeviceMemory
  {
    return m_pimpl->image_memory.get();
  }

  auto rgba32f_composition_pass::frame_buffer() const -> vk::Framebuffer
  {
    return m_pimpl->frame_buffer.get();
  }

  auto rgba32f_composition_pass::command_pool() const -> vk::CommandPool
  {
    return m_pimpl->command_pool.get();
  }

  auto rgba32f_composition_pass::command_buffer() const -> vk::CommandBuffer
  {
    return m_pimpl->command_buffer.get();
  }

  auto rgba32f_composition_pass::begin_draw() -> vk::CommandBuffer
  {
    return m_pimpl->begin_draw();
  }

  void rgba32f_composition_pass::end_draw()
  {
    m_pimpl->end_draw();
  }

  void rgba32f_composition_pass::wait_draw()
  {
    m_pimpl->wait_draw();
  }
} // namespace yave::vulkan