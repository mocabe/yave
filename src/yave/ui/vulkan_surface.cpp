//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/vulkan_surface.hpp>
#include <yave/ui/glfw_window.hpp>

#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(ui::vulkan_surface);

namespace {

  auto createWindowSurface(GLFWwindow* window, const vk::Instance& instance)
    -> vk::UniqueSurfaceKHR
  {
    VkSurfaceKHR surface;

    auto err = glfwCreateWindowSurface(instance, window, nullptr, &surface);

    if (err != VK_SUCCESS) {
      throw std::runtime_error("Failed to create window surface");
    }

    if (!surface)
      throw std::runtime_error("Failed to create window surface");

    vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderStatic> deleter(instance);
    return vk::UniqueSurfaceKHR(surface, deleter);
  }

  auto chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
    -> vk::SurfaceFormatKHR
  {
    auto default_format       = vk::SurfaceFormatKHR();
    default_format.format     = vk::Format::eB8G8R8A8Unorm;
    default_format.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

    if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
      // use default format
      return default_format;
    }

    for (auto&& format : formats) {
      // find default format
      if (format == default_format)
        return default_format;
    }

    // fallback to first format
    return formats[0];
  }

  auto choosePresentMode(const std::vector<vk::PresentModeKHR>& modes)
    -> vk::PresentModeKHR
  {
    for (auto&& mode : modes) {
      if (mode == vk::PresentModeKHR::eMailbox)
        return mode;
      if (mode == vk::PresentModeKHR::eImmediate)
        return mode;
    }
    return vk::PresentModeKHR::eFifo;
  }

  auto chooseSwapchainExtent(
    vk::Extent2D windowSize,
    const vk::SurfaceCapabilitiesKHR& capabilities) -> vk::Extent2D
  {
    vk::Extent2D extent = capabilities.currentExtent;

    if (extent.width == std::numeric_limits<uint32_t>::max()) {
      extent.width = std::clamp(
        windowSize.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width);
    }

    if (extent.height == std::numeric_limits<uint32_t>::max()) {
      extent.height = std::clamp(
        windowSize.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height);
    }

    return extent;
  }

  auto chooseSwapchainPreTransform(
    const vk::SurfaceCapabilitiesKHR& capabilities)
    -> vk::SurfaceTransformFlagBitsKHR
  {
    return capabilities.currentTransform;
  }

  auto chooseSwapchainCompositeAlpha(
    const vk::SurfaceCapabilitiesKHR& capabilities)
    -> vk::CompositeAlphaFlagBitsKHR
  {
    auto supported = capabilities.supportedCompositeAlpha;

    if (supported & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
      return vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;

    if (supported & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
      return vk::CompositeAlphaFlagBitsKHR::ePostMultiplied;

    if (supported & vk::CompositeAlphaFlagBitsKHR::eInherit)
      return vk::CompositeAlphaFlagBitsKHR::eInherit;

    if (supported & vk::CompositeAlphaFlagBitsKHR::eOpaque)
      return vk::CompositeAlphaFlagBitsKHR::eOpaque;

    throw std::runtime_error("No supported composite alpha option");
  }

  auto createSwapchain(
    const vk::SurfaceKHR& surface,
    const vk::Extent2D windowExtent,
    uint32_t graphicsQueueIndex,
    uint32_t presentQueueIndex,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& logicalDevice,
    const vk::SwapchainKHR& oldSwapchain,
    vk::SurfaceFormatKHR* out_format,
    vk::PresentModeKHR* out_present_mode,
    vk::Extent2D* out_extent,
    uint32_t* out_image_count) -> vk::UniqueSwapchainKHR
  {
    if (!physicalDevice.getSurfaceSupportKHR(presentQueueIndex, surface)) {
      throw std::runtime_error(
        "Current surface format is not supported by presentation queue "
        "family");
    }

    auto availFormats = physicalDevice.getSurfaceFormatsKHR(surface);
    auto format       = chooseSurfaceFormat(availFormats);

    auto availModes = physicalDevice.getSurfacePresentModesKHR(surface);
    auto mode       = choosePresentMode(availModes);

    auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    auto extent       = chooseSwapchainExtent(windowExtent, capabilities);

    // maxImageCount is 0 when infinite...
    auto imageCount = std::clamp(
      capabilities.minImageCount + 1,
      capabilities.minImageCount,
      capabilities.maxImageCount == 0 ? std::numeric_limits<uint32_t>::max()
                                      : capabilities.maxImageCount);

    auto preTransform   = chooseSwapchainPreTransform(capabilities);
    auto compositeAlpha = chooseSwapchainCompositeAlpha(capabilities);
    auto imageUsage     = vk::ImageUsageFlagBits::eColorAttachment;

    auto info = vk::SwapchainCreateInfoKHR()
                  .setSurface(surface)
                  .setMinImageCount(imageCount)
                  .setImageFormat(format.format)
                  .setImageColorSpace(format.colorSpace)
                  .setImageExtent(extent)
                  .setPreTransform(preTransform)
                  .setCompositeAlpha(compositeAlpha)
                  .setPresentMode(mode)
                  .setClipped(VK_TRUE)
                  .setOldSwapchain(oldSwapchain)
                  .setImageArrayLayers(1)
                  .setImageUsage(imageUsage);

    auto queueFamilyIndicies =
      std::array {graphicsQueueIndex, presentQueueIndex};

    if (graphicsQueueIndex == presentQueueIndex) {
      info.setImageSharingMode(vk::SharingMode::eExclusive);
    } else {
      info.setImageSharingMode(vk::SharingMode::eExclusive);
      info.setQueueFamilyIndices(queueFamilyIndicies);
    }

    // create swapchain

    auto swapchain = logicalDevice.createSwapchainKHRUnique(info);

    // write out pointers
    *out_format       = format;
    *out_present_mode = mode;
    *out_extent       = extent;
    *out_image_count  = imageCount;

    return swapchain;
  }

  auto createSwapchainImageViews(
    const std::vector<vk::Image>& swapchain_images,
    const vk::SurfaceFormatKHR& surface_format,
    const vk::Device& device) -> std::vector<vk::UniqueImageView>
  {
    auto componentMapping = vk::ComponentMapping(
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity);

    auto subResourceRange =
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

    std::vector<vk::UniqueImageView> ret;
    ret.reserve(swapchain_images.size());

    for (auto&& image : swapchain_images) {

      auto info = vk::ImageViewCreateInfo()
                    .setImage(image)
                    .setFormat(surface_format.format)
                    .setViewType(vk::ImageViewType::e2D)
                    .setComponents(componentMapping)
                    .setSubresourceRange(subResourceRange);

      ret.push_back(device.createImageViewUnique(info));
    }
    return ret;
  }

  auto createRenderPass(
    const vk::SurfaceFormatKHR& swapchain_format,
    const vk::Device& device) -> vk::UniqueRenderPass
  {
    auto attachments =
      std::array {vk::AttachmentDescription()
                    .setFormat(swapchain_format.format)
                    .setSamples(vk::SampleCountFlagBits::e1)
                    // behaviour before rendering
                    // eClear   : clear
                    // eLoad    : preserve
                    // eDontCare: undefined
                    .setLoadOp(vk::AttachmentLoadOp::eClear)
                    // behaviour before rendering
                    // eStore   : store rendered content to memory
                    // eDontCare: undefined
                    .setStoreOp(vk::AttachmentStoreOp::eStore)
                    // don't care about stencil
                    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                    // don't care initial layout (we clear it anyway)
                    .setInitialLayout(vk::ImageLayout::eUndefined)
                    // pass it to swap chain
                    .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)};

    auto colorAttachmentRef =
      std::array {vk::AttachmentReference() //
                    .setAttachment(0)       //
                    .setLayout(vk::ImageLayout::eColorAttachmentOptimal)};

    auto subpasses =
      std::array {vk::SubpassDescription()
                    .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                    .setColorAttachments(colorAttachmentRef)};

    auto dependencies = std::array {
      vk::SubpassDependency()
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstAccessMask(
          vk::AccessFlagBits::eColorAttachmentRead
          | vk::AccessFlagBits::eColorAttachmentWrite)};

    auto info = vk::RenderPassCreateInfo()
                  .setAttachments(attachments)
                  .setSubpasses(subpasses)
                  .setDependencies(dependencies);

    return device.createRenderPassUnique(info);
  }

  auto createCommandPool(uint32_t graphicsQueueIndex, const vk::Device& device)
    -> vk::UniqueCommandPool
  {
    auto info = vk::CommandPoolCreateInfo()
                  // allow vkResetCommandBuffer
                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                  // use graphics queue
                  .setQueueFamilyIndex(graphicsQueueIndex);

    return device.createCommandPoolUnique(info);
  }

  auto createCommandBuffers(
    uint32_t size,
    const vk::CommandBufferLevel& level,
    const vk::CommandPool& commandPool,
    const vk::Device& device) -> std::vector<vk::UniqueCommandBuffer>
  {
    auto info = vk::CommandBufferAllocateInfo()
                  .setCommandPool(commandPool)
                  .setLevel(level)
                  .setCommandBufferCount(size);

    return device.allocateCommandBuffersUnique(info);
  }

  auto createFrameBuffers(
    const std::vector<vk::UniqueImageView>& image_views,
    const vk::RenderPass& render_pass,
    const vk::Extent2D swapchainExtent,
    const vk::Device& device) -> std::vector<vk::UniqueFramebuffer>
  {
    std::vector<vk::UniqueFramebuffer> ret;
    ret.reserve(image_views.size());

    for (auto&& view : image_views) {

      auto attachments = std::array {view.get()};

      auto info = vk::FramebufferCreateInfo()
                    .setRenderPass(render_pass)
                    .setAttachments(attachments)
                    .setWidth(swapchainExtent.width)
                    .setHeight(swapchainExtent.height)
                    .setLayers(1);

      ret.push_back(device.createFramebufferUnique(info));
    }
    return ret;
  }

  auto createSemaphore(const vk::Device& device) -> vk::UniqueSemaphore
  {
    auto info = vk::SemaphoreCreateInfo();
    return device.createSemaphoreUnique(info);
  }

  auto createSemaphores(uint32_t size, const vk::Device& device)
    -> std::vector<vk::UniqueSemaphore>
  {
    std::vector<vk::UniqueSemaphore> ret;
    ret.reserve(size);
    for (uint32_t i = 0; i < size; ++i) {
      ret.push_back(createSemaphore(device));
    }
    return ret;
  }

  auto createFence(const vk::Device& device, const vk::FenceCreateFlags& flags)
    -> vk::UniqueFence
  {
    auto info = vk::FenceCreateInfo().setFlags(flags);
    return device.createFenceUnique(info);
  }

  auto createFences(
    uint32_t size,
    const vk::Device& device,
    const vk::FenceCreateFlags& flags) -> std::vector<vk::UniqueFence>
  {
    std::vector<vk::UniqueFence> ret;
    ret.reserve(size);
    for (uint32_t i = 0; i < size; ++i) {
      ret.push_back(createFence(device, flags));
    }
    return ret;
  }

} // namespace

namespace yave::ui {

  vulkan_surface::vulkan_surface(vulkan_device& device, glfw_window& win)
    : m_device {device}
    , m_win {win}
  {
    m_surface = createWindowSurface(m_win.handle(), m_device.instance());

    auto fbSize = m_win.fb_size();

    m_swapchain = createSwapchain(
      m_surface.get(),
      vk::Extent2D {
        static_cast<uint32_t>(fbSize.w), static_cast<uint32_t>(fbSize.h)},
      m_device.graphics_queue_index(),
      m_device.present_queue_index(),
      m_device.physical_device(),
      m_device.device(),
      m_swapchain.get(),
      &m_swapchain_format,
      &m_swapchain_present_mode,
      &m_swapchain_extent,
      &m_swapchain_image_count);

    m_swapchain_images =
      m_device.device().getSwapchainImagesKHR(m_swapchain.get());

    m_swapchain_image_views = createSwapchainImageViews(
      m_swapchain_images, m_swapchain_format, m_device.device());

    assert(m_swapchain_images.size() == m_swapchain_image_views.size());
    assert(m_swapchain_images.size() == m_swapchain_image_count);

    m_render_pass = createRenderPass(m_swapchain_format, m_device.device());
    m_command_pool =
      createCommandPool(m_device.graphics_queue_index(), m_device.device());

    m_command_buffers = createCommandBuffers(
      m_swapchain_image_count,
      vk::CommandBufferLevel::ePrimary,
      m_command_pool.get(),
      m_device.device());

    m_frame_buffers = createFrameBuffers(
      m_swapchain_image_views,
      m_render_pass.get(),
      m_swapchain_extent,
      m_device.device());

    assert(m_command_buffers.size() == m_frame_buffers.size());

    m_acquire_semaphores =
      createSemaphores(m_swapchain_image_count, device.device());
    m_complete_semaphores =
      createSemaphores(m_swapchain_image_count, device.device());

    // create with signaled state
    m_in_flight_fences = createFences(
      m_swapchain_image_count,
      device.device(),
      vk::FenceCreateFlagBits::eSignaled);

    m_acquire_fence = createFence(device.device(), vk::FenceCreateFlags());
  }

  vulkan_surface::~vulkan_surface() noexcept
  {
    m_device.wait_idle();
  }

  void vulkan_surface::set_clear_color(float r, float g, float b, float a)
  {
    m_clear_color.setFloat32({r, g, b, a});
  }

  auto vulkan_surface::swapchain_extent() const -> vk::Extent2D
  {
    return m_swapchain_extent;
  }

  bool vulkan_surface::rebuild_required() const
  {
    auto wSize   = m_win.fb_size();
    auto wExtent = vk::Extent2D {
      static_cast<uint32_t>(wSize.w), static_cast<uint32_t>(wSize.h)};
    return m_swapchain_extent != wExtent;
  }

  void vulkan_surface::rebuild()
  {
    // FIXME: ????
    // Windows: minimized window have zero extent. Wait until next event. 
    // while (vk::Extent2D(newWindowExtent) == vk::Extent2D(0, 0)) {
    //   glfwWaitEvents();
    //   newWindowExtent = window_extent.load();
    // }

    // wait idle
    m_device.wait_idle();

    /* destroy swapchain resources */
    m_in_flight_fences.clear();
    m_complete_semaphores.clear();
    m_acquire_semaphores.clear();
    m_command_buffers.clear();
    m_frame_buffers.clear();
    m_swapchain_image_views.clear();
    m_swapchain_images.clear();

    /* reset index */
    m_frame_index = 0;
    m_image_index = 0;

    /* create new swapchain resources */

    // load extent to get accurate value at this point
    auto windowSize = m_win.fb_size();

    log_info(
      "*** rebuilding frame buffers {},{} ***", windowSize.w, windowSize.h);

    m_swapchain = createSwapchain(
      m_surface.get(),
      vk::Extent2D {
        static_cast<uint32_t>(windowSize.w),
        static_cast<uint32_t>(windowSize.h)},
      m_device.graphics_queue_index(),
      m_device.present_queue_index(),
      m_device.physical_device(),
      m_device.device(),
      m_swapchain.get(),
      &m_swapchain_format,
      &m_swapchain_present_mode,
      &m_swapchain_extent,
      &m_swapchain_image_count);

    m_swapchain_images =
      m_device.device().getSwapchainImagesKHR(m_swapchain.get());

    m_swapchain_image_views = createSwapchainImageViews(
      m_swapchain_images, m_swapchain_format, m_device.device());

    m_command_buffers = createCommandBuffers(
      m_swapchain_image_count,
      vk::CommandBufferLevel::ePrimary,
      m_command_pool.get(),
      m_device.device());

    m_frame_buffers = createFrameBuffers(
      m_swapchain_image_views,
      m_render_pass.get(),
      m_swapchain_extent,
      m_device.device());

    m_acquire_semaphores =
      createSemaphores(m_swapchain_image_count, m_device.device());

    m_complete_semaphores =
      createSemaphores(m_swapchain_image_count, m_device.device());

    m_in_flight_fences = createFences(
      m_swapchain_image_count,
      m_device.device(),
      vk::FenceCreateFlagBits::eSignaled);
  }

  void vulkan_surface::begin_frame()
  {
    // set next frame index
    m_frame_index = (m_frame_index + 1) % m_swapchain_image_count;

    auto device = m_device.device();

    {
      // acquire next image, get next image_index.
      // when swapchain is out of date, rebuild it until not getting error.
      // VkAcquireNextImageKHR can be non-blocking depending on driver
      // implementation. so uses fence to prevent over-commit.

      // check if window is resized
      if (rebuild_required())
        rebuild();

      // try without fence first time
      auto err = device.acquireNextImageKHR(
        m_swapchain.get(),
        std::numeric_limits<uint64_t>::max(),
        m_acquire_semaphores[m_frame_index].get(),
        vk::Fence(),
        &m_image_index);

      // loop
      while (err == vk::Result::eErrorOutOfDateKHR) {

        rebuild();

        device.resetFences(m_acquire_fence.get());

        err = device.acquireNextImageKHR(
          m_swapchain.get(),
          std::numeric_limits<uint64_t>::max(),
          m_acquire_semaphores[m_frame_index].get(),
          m_acquire_fence.get(),
          &m_image_index);

        auto wait_err = device.waitForFences(
          m_acquire_fence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());

        if (wait_err != vk::Result::eSuccess)
          throw std::runtime_error(
            "Failed to wait for fence: " + vk::to_string(wait_err));
      }

      if (err != vk::Result::eSuccess)
        throw std::runtime_error(
          "Failed to acquire image: " + vk::to_string(err));
    }

    {
      // wait in-flight fence. this should happen after acquiring next image
      // because rebuilding frame buffers may reset all fences to signaled
      // state.

      auto err = device.waitForFences(
        m_in_flight_fences[m_frame_index].get(),
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

      if (err != vk::Result::eSuccess)
        throw std::runtime_error(
          "Failed to wait for in-flight fence: " + vk::to_string(err));

      // reset fence
      device.resetFences(m_in_flight_fences[m_frame_index].get());
    }
  }

  void vulkan_surface::end_frame()
  {
    auto waitSemaphores =
      std::array {m_acquire_semaphores[m_frame_index].get()};

    auto signalSemaphores =
      std::array {m_complete_semaphores[m_frame_index].get()};

    auto waitStage = std::array {vk::PipelineStageFlags(
      vk::PipelineStageFlagBits::eColorAttachmentOutput)};

    auto commandBuffers = std::array {m_command_buffers[m_frame_index].get()};

    /* submit current command buffers */
    {
      auto submitInfo = vk::SubmitInfo()
                          .setWaitSemaphores(waitSemaphores)
                          .setSignalSemaphores(signalSemaphores)
                          .setWaitDstStageMask(waitStage)
                          .setCommandBuffers(commandBuffers);

      // submit
      auto err = m_device.graphics_queue().submit(
        1, &submitInfo, m_in_flight_fences[m_frame_index].get());

      if (err != vk::Result::eSuccess)
        throw std::runtime_error(
          "Failed to submit command buffer: " + vk::to_string(err));
    }

    /* present result */
    {
      auto swapchain  = std::array {m_swapchain.get()};
      auto imageIndex = std::array {m_image_index};

      auto presentInfo = vk::PresentInfoKHR()
                           .setWaitSemaphores(signalSemaphores)
                           .setSwapchains(swapchain)
                           .setImageIndices(imageIndex);

      auto err = m_device.present_queue().presentKHR(&presentInfo);

      // Surface is not longer compatible with current frame buffer.
      if (err == vk::Result::eErrorOutOfDateKHR) {
        log_info("eErrorOutOfDataKHR on present command");
        return;
      }

      if (err != vk::Result::eSuccess)
        throw std::runtime_error("Failed to present: " + vk::to_string(err));
    }
  }

  auto vulkan_surface::begin_record() -> vk::CommandBuffer
  {
    auto buffer = m_command_buffers[m_frame_index].get();

    /* begin command buffer */
    {
      vk::CommandBufferBeginInfo beginInfo;
      beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
      buffer.begin(beginInfo);
    }
    /* begin render pass */
    {
      // you can set clear color with set_clear_color()
      auto clearValue = std::array {vk::ClearValue().setColor(m_clear_color)};

      // begin render pass
      auto beginInfo =
        vk::RenderPassBeginInfo()
          .setRenderPass(m_render_pass.get())
          .setFramebuffer(m_frame_buffers[m_image_index].get())
          .setRenderArea(vk::Rect2D().setExtent(m_swapchain_extent))
          .setClearValues(clearValue);

      buffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
    }
    return buffer;
  }

  void vulkan_surface::end_record(const vk::CommandBuffer& buffer)
  {
    buffer.endRenderPass();
    buffer.end();
  }

  auto vulkan_surface::swapchain_index() const -> uint32_t
  {
    return m_image_index;
  }

  auto vulkan_surface::swapchain_index_count() const -> uint32_t
  {
    return m_swapchain_image_count;
  }

  auto vulkan_surface::frame_index() const -> uint32_t
  {
    return m_frame_index;
  }

  auto vulkan_surface::frame_index_count() const -> uint32_t
  {
    return m_swapchain_image_count;
  }

} // namespace yave::ui
