//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/composition_pass.hpp>
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
    vk::UniqueCommandPool command_pool;
    vk::UniqueCommandBuffer command_buffer;

  public: /* pipeline */
    vk::UniqueRenderPass render_pass;
    offscreen_frame_data frame_data;

  public:
    // fence to wait submitted queue operations finish
    vk::UniqueFence submit_fence;

  public:
    composition_pass_impl(uint32_t width, uint32_t height, vulkan_context& ctx)
      : context {ctx}
    {
      init_logger();

      auto extent = vk::Extent2D {width, height};
      auto format = gilToVkFormat(pixel_type());

      // create render pass
      render_pass = createRenderPass(format, ctx.device());

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

      frame_data = create_offscreen_frame_data(
        extent,
        format,
        ctx.graphics_queue(),
        command_pool.get(),
        render_pass.get(),
        ctx.device(),
        ctx.physical_device());

      clear_offscreen_frame_data(
        frame_data,
        vk::ClearColorValue(),
        ctx.graphics_queue(),
        command_pool.get(),
        ctx.device(),
        ctx.physical_device());
    }

    ~composition_pass_impl() noexcept
    {
      context.device().waitIdle();
    }

    void store_frame(
      const boost::gil::image_view<typename pixel_loc_type::const_t>& view)
    {
      if (
        frame_data.extent
        != vk::Extent2D {static_cast<uint32_t>(view.width()),
                         static_cast<uint32_t>(view.height())})
        throw std::runtime_error("Incompatible size for frame buffer");

      assert(view.is_1d_traversable());

      wait_draw();

      store_offscreen_frame_data(
        frame_data,
        reinterpret_cast<const std::byte*>(view.row_begin(0)),
        view.size() * sizeof(pixel_type),
        context.graphics_queue(),
        command_pool.get(),
        context.device(),
        context.physical_device());
    }

    void load_frame(const boost::gil::image_view<pixel_loc_type>& view)
    {
      if (
        frame_data.extent
        != vk::Extent2D {static_cast<uint32_t>(view.width()),
                         static_cast<uint32_t>(view.height())})
        throw std::runtime_error("Incompatible size for frame buffer");

      assert(view.is_1d_traversable());

      wait_draw();

      load_offscreen_frame_data(
        frame_data,
        reinterpret_cast<std::byte*>(view.row_begin(0)),
        view.size() * sizeof(pixel_type),
        context.graphics_queue(),
        command_pool.get(),
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
        info.renderArea.extent = frame_data.extent;
        info.framebuffer       = frame_data.buffer.get();

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

  auto rgba32f_composition_pass::width() const noexcept -> uint32_t
  {
    return m_pimpl->frame_data.extent.width;
  }

  auto rgba32f_composition_pass::height() const noexcept -> uint32_t
  {
    return m_pimpl->frame_data.extent.height;
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
    return m_pimpl->frame_data.extent;
  }

  auto rgba32f_composition_pass::frame_image() const -> vk::Image
  {
    return m_pimpl->frame_data.image.get();
  }

  auto rgba32f_composition_pass::frame_image_view() const -> vk::ImageView
  {
    return m_pimpl->frame_data.view.get();
  }

  auto rgba32f_composition_pass::frame_format() const -> vk::Format
  {
    return m_pimpl->frame_data.format;
  }

  auto rgba32f_composition_pass::frame_memory() const -> vk::DeviceMemory
  {
    return m_pimpl->frame_data.memory.get();
  }

  auto rgba32f_composition_pass::frame_buffer() const -> vk::Framebuffer
  {
    return m_pimpl->frame_data.buffer.get();
  }

  auto rgba32f_composition_pass::command_pool() const -> vk::CommandPool
  {
    return m_pimpl->command_pool.get();
  }

  auto rgba32f_composition_pass::command_buffer() const -> vk::CommandBuffer
  {
    return m_pimpl->command_buffer.get();
  }

  auto rgba32f_composition_pass::render_pass() const -> vk::RenderPass
  {
    return m_pimpl->render_pass.get();
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