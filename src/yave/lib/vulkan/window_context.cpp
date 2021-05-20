//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#define GLFW_INCLUDE_VULKAN

#include <yave/lib/vulkan/window_context.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(vulkan)

namespace {

  auto getGraphicsQueueIndex(const vk::PhysicalDevice& physicalDevice)
    -> uint32_t
  {
    auto properties = physicalDevice.getQueueFamilyProperties();

    for (uint32_t i = 0; i < properties.size(); ++i) {
      if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics)
        return i;
    }

    throw std::runtime_error("Device does not support graphics queue");
  }

  auto getPresentQueueIndex(
    uint32_t graphicsQueueIndex,
    const vk::Instance& instance,
    const vk::PhysicalDevice& physicalDevice) -> uint32_t
  {
    using namespace yave;

    // when graphics queue supports presentation, use graphics queue.
    if (glfwGetPhysicalDevicePresentationSupport(
          instance, physicalDevice, graphicsQueueIndex)) {
      return graphicsQueueIndex;
    }

    log_warning( "Graphics queue does not support presentation.");

    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index) {
      auto support = glfwGetPhysicalDevicePresentationSupport(
        instance, physicalDevice, index);

      if (support)
        return index;
    }

    throw std::runtime_error("Presentation is not supported on this device");
  }

  auto getDeviceExtensions() -> std::vector<std::string>
  {
    // default extensions
    constexpr std::array extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, // for swapchain
    };

    std::vector<std::string> ret;

    ret.insert(ret.end(), extensions.begin(), extensions.end());

    return ret;
  }

  auto getDeviceLayers() -> std::vector<std::string>
  {
    return {};
  }

  auto getDeviceQueue(uint32_t queueFamilyIndex, const vk::Device& device)
    -> vk::Queue
  {
    // Assume only single queue is initialized.
    return device.getQueue(queueFamilyIndex, 0);
  }

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

  auto getWindowExtent(GLFWwindow* window) -> vk::Extent2D
  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    if (width < 0)
      width = 0;
    if (height < 0)
      height = 0;

    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }

  auto chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
    -> vk::SurfaceFormatKHR
  {
    using namespace yave;

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

    log_warning(
      "Default surface format is not avalable. Return first format "
      "avalable.");

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
    uint32_t imageCount = std::clamp(
      capabilities.minImageCount + 1,
      capabilities.minImageCount,
      capabilities.maxImageCount == 0 ? std::numeric_limits<uint32_t>::max()
                                      : capabilities.maxImageCount);

    auto preTransform   = chooseSwapchainPreTransform(capabilities);
    auto compositeAlpha = chooseSwapchainCompositeAlpha(capabilities);

    vk::SwapchainCreateInfoKHR info;
    info.flags           = vk::SwapchainCreateFlagsKHR();
    info.surface         = surface;
    info.minImageCount   = imageCount;
    info.imageFormat     = format.format;
    info.imageColorSpace = format.colorSpace;
    info.imageExtent     = extent;
    info.preTransform    = preTransform;
    info.compositeAlpha  = compositeAlpha;
    info.presentMode     = mode;
    info.clipped         = VK_TRUE;
    info.oldSwapchain    = oldSwapchain;
    // single layer
    info.imageArrayLayers = 1;
    // directly render (as color attachment)
    info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    std::array queueFamilyIndicies = {graphicsQueueIndex, presentQueueIndex};

    if (graphicsQueueIndex == presentQueueIndex) {
      info.imageSharingMode      = vk::SharingMode::eExclusive;
      info.queueFamilyIndexCount = 0;
      info.pQueueFamilyIndices   = nullptr;
    } else {
      info.imageSharingMode      = vk::SharingMode::eConcurrent;
      info.queueFamilyIndexCount = queueFamilyIndicies.size();
      info.pQueueFamilyIndices   = queueFamilyIndicies.data();
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

  auto chooseImageViewComponentMapping() -> vk::ComponentMapping
  {
    return vk::ComponentMapping(
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity);
  }

  auto chooseImageViewSubResourceRange() -> vk::ImageSubresourceRange
  {
    return vk::ImageSubresourceRange(
      vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
  }

  auto createSwapchainImageViews(
    const vk::SwapchainKHR& swapchain,
    const vk::SurfaceFormatKHR& surface_format,
    const vk::Device& device) -> std::vector<vk::UniqueImageView>
  {
    auto componentMapping = chooseImageViewComponentMapping();
    auto subResourceRange = chooseImageViewSubResourceRange();
    auto swapchainImages  = device.getSwapchainImagesKHR(swapchain);

    std::vector<vk::UniqueImageView> ret;
    ret.reserve(swapchainImages.size());

    for (auto&& image : swapchainImages) {
      vk::ImageViewCreateInfo info;
      info.flags            = vk::ImageViewCreateFlags();
      info.image            = image;
      info.format           = surface_format.format;
      info.viewType         = vk::ImageViewType::e2D;
      info.components       = componentMapping;
      info.subresourceRange = subResourceRange;

      ret.push_back(device.createImageViewUnique(info));
    }

    return ret;
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

      auto attachments = std::vector {view.get()};

      vk::FramebufferCreateInfo info;
      info.flags           = vk::FramebufferCreateFlags();
      info.renderPass      = render_pass;
      info.attachmentCount = (uint32_t)attachments.size();
      info.pAttachments    = attachments.data();
      info.width           = swapchainExtent.width;
      info.height          = swapchainExtent.height;
      info.layers          = 1;

      ret.push_back(device.createFramebufferUnique(info));
    }

    return ret;
  }

  auto getRenderPassColorAttachments(
    const vk::SurfaceFormatKHR& swapchain_format)
    -> std::array<vk::AttachmentDescription, 1>
  {
    vk::AttachmentDescription colorAttachment;

    // swap chain image format
    colorAttachment.format = swapchain_format.format;
    // sample count
    colorAttachment.samples = vk::SampleCountFlagBits::e1;

    // behaviour before rendering
    // eClear   : clear
    // eLoad    : preserve
    // eDontCare: undefined
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;

    // behaviour before rendering
    // eStore   : store rendered content to memory
    // eDontCare: undefined
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;

    // dont care about stencil
    colorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    // don't care initial layout (we clear it anyway)
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    // pass it to swap chain
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    return {colorAttachment};
  }

  auto getRenderPassAttachmentReferences()
    -> std::vector<vk::AttachmentReference>
  {
    vk::AttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;
    return {colorAttachmentRef};
  }

  std::pair<
    std::vector<vk::SubpassDescription>,
    std::tuple<std::vector<vk::AttachmentReference>>>
    getSubpasses()
  {
    std::vector<vk::AttachmentReference> colorAttachmentRef =
      getRenderPassAttachmentReferences();

    std::vector<vk::SubpassDescription> subpass(1);
    subpass[0].pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
    subpass[0].colorAttachmentCount = (uint32_t)colorAttachmentRef.size();
    subpass[0].pColorAttachments    = colorAttachmentRef.data();

    // forward resource to caller with std::make_*
    return std::make_pair(
      subpass, std::make_tuple(std::move(colorAttachmentRef)));
  }

  auto getSubpassDependency() -> std::array<vk::SubpassDependency, 1>
  {
    vk::SubpassDependency dep;
    dep.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dep.dstSubpass    = 0;
    dep.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dep.srcAccessMask = vk::AccessFlags();
    dep.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead
                        | vk::AccessFlagBits::eColorAttachmentWrite;
    return {dep};
  }

  auto createRenderPass(
    const vk::SurfaceFormatKHR& swapchain_format,
    const vk::Device& device) -> vk::UniqueRenderPass
  {
    using namespace yave;

    vk::RenderPassCreateInfo info;

    auto attachments     = getRenderPassColorAttachments(swapchain_format);
    info.attachmentCount = (uint32_t)attachments.size();
    info.pAttachments    = attachments.data();

    auto [subpasses, subpassesResource] = getSubpasses();
    info.subpassCount                   = (uint32_t)subpasses.size();
    info.pSubpasses                     = subpasses.data();
    (void)subpassesResource;

    auto dependency      = getSubpassDependency();
    info.dependencyCount = (uint32_t)dependency.size();
    info.pDependencies   = dependency.data();

    return device.createRenderPassUnique(info);
  }

  auto createCommandPool(uint32_t graphicsQueueIndex, const vk::Device& device)
    -> vk::UniqueCommandPool
  {
    vk::CommandPoolCreateInfo info;
    // allow vkResetCommandBuffer
    info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    // use graphics queue
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

  auto createSemaphore(const vk::Device& device) -> vk::UniqueSemaphore
  {
    vk::SemaphoreCreateInfo info;
    info.flags = vk::SemaphoreCreateFlags();
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
    vk::FenceCreateInfo info;
    info.flags = flags;
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

namespace yave::vulkan {

  // -----------------------------------------
  // window_context

  class window_context::impl
  {
  public:
    vulkan_context& vulkan_ctx;  // non-owning
    glfw::glfw_window& glfw_win; // non-owning

  public:
    vk::UniqueDevice device;
    vk::UniqueSurfaceKHR surface;

  public:
    uint32_t graphics_queue_index;
    uint32_t present_queue_index;
    vk::Queue graphics_queue;
    vk::Queue present_queue;

  public:
    vk::UniqueRenderPass render_pass;
    vk::UniqueCommandPool command_pool;
    vk::UniquePipelineCache pipeline_cache;
    vk::UniqueSwapchainKHR swapchain;

  public:
    std::vector<vk::Image> swapchain_images; // owned by swapchain
    std::vector<vk::UniqueImageView> swapchain_image_views;
    std::vector<vk::UniqueCommandBuffer> command_buffers;
    std::vector<vk::UniqueFramebuffer> frame_buffers;
    std::vector<vk::UniqueSemaphore> acquire_semaphores;
    std::vector<vk::UniqueSemaphore> complete_semaphores;
    std::vector<vk::UniqueFence> in_flight_fences;
    vk::UniqueFence acquire_fence;
    vk::ClearColorValue clearColor;

  public:
    vk::SurfaceFormatKHR swapchain_format;
    vk::PresentModeKHR swapchain_present_mode;
    vk::Extent2D swapchain_extent;
    uint32_t swapchain_image_count;

  public:
    uint32_t image_index = 0;
    uint32_t frame_index = 0;

  public:
    // GLFW window size callback will update this on window resize
    std::atomic<VkExtent2D> window_extent;

  public:
    impl(vulkan_context& vk_ctx, glfw::glfw_window& glfw_win);
    ~impl() noexcept;

  public:
    void rebuild_frame_buffers();
    void begin_frame();
    void end_frame();
    auto begin_record() -> vk::CommandBuffer;
    void end_record(const vk::CommandBuffer&);

  public:
    bool resized() const;
    bool should_close() const;
    void set_clear_color(float r, float g, float b, float a);
    auto single_time_command() const -> vulkan::single_time_command;
  };

  window_context::impl::impl(vulkan_context& ctx, glfw::glfw_window& win)
    : vulkan_ctx {ctx}
    , glfw_win {win}
  {
    log_info("Initializing window context");

    if (!glfwVulkanSupported())
      throw std::runtime_error("GLFW could not find Vulkan");

    // get window size
    window_extent = getWindowExtent(win.get());

    // key for user data pointer of window context
    static constexpr const char* wd_key = "widnow_context";

    // set user data
    if (!glfw::glfw_window::add_user_data(win.get(), wd_key, this)) {
      throw std::runtime_error("Failed to set window data");
    }

    // set window resize callback
    glfwSetFramebufferSizeCallback(
      win.get(), [](GLFWwindow* w, int width, int height) noexcept {
        if (
          auto* ctx = (window_context::impl*)glfw::glfw_window::get_user_data(
            w, wd_key)) {
          // set new window size
          ctx->window_extent = {
            static_cast<uint32_t>(std::clamp(0, width, width)),
            static_cast<uint32_t>(std::clamp(0, height, height))};
        }
      });

    // create surface
    surface = createWindowSurface(win.get(), ctx.instance());

    // get queue index

    graphics_queue_index = getGraphicsQueueIndex(ctx.physical_device());
    log_info("Graphs queue index: {}", graphics_queue_index);

    present_queue_index = getPresentQueueIndex(
      graphics_queue_index, ctx.instance(), ctx.physical_device());
    log_info("Presentation queue index: {}", present_queue_index);

    // create logical device
    device = ctx.create_device(
      {graphics_queue_index, present_queue_index},
      getDeviceExtensions(),
      getDeviceLayers());

    // queue
    graphics_queue = getDeviceQueue(graphics_queue_index, device.get());
    present_queue  = getDeviceQueue(present_queue_index, device.get());

    // create swapchain
    swapchain = createSwapchain(
      surface.get(),
      {window_extent},
      graphics_queue_index,
      present_queue_index,
      ctx.physical_device(),
      device.get(),
      swapchain.get(),
      &swapchain_format,       // out
      &swapchain_present_mode, // out
      &swapchain_extent,       // out
      &swapchain_image_count); // out

    // get swapchain images
    swapchain_images = device->getSwapchainImagesKHR(swapchain.get());

    // create image views
    swapchain_image_views = createSwapchainImageViews(
      swapchain.get(), swapchain_format, device.get());

    assert(swapchain_images.size() == swapchain_image_views.size());
    assert(swapchain_images.size() == swapchain_image_count);

    // create render pass
    render_pass = createRenderPass(swapchain_format, device.get());

    // create frame buffers
    frame_buffers = createFrameBuffers(
      swapchain_image_views, render_pass.get(), swapchain_extent, device.get());

    // create command pool
    command_pool = createCommandPool(graphics_queue_index, device.get());

    // create command buffers
    command_buffers = createCommandBuffers(
      swapchain_image_count,
      vk::CommandBufferLevel::ePrimary,
      command_pool.get(),
      device.get());

    assert(command_buffers.size() == frame_buffers.size());

    // semaphores
    acquire_semaphores  = createSemaphores(swapchain_image_count, device.get());
    complete_semaphores = createSemaphores(swapchain_image_count, device.get());

    // create with signaled state
    in_flight_fences = createFences(
      swapchain_image_count, device.get(), vk::FenceCreateFlagBits::eSignaled);

    acquire_fence = createFence(device.get(), vk::FenceCreateFlags());

    log_info("Initialized window context");
  }

  window_context::impl::~impl() noexcept
  {
    device->waitIdle();
    log_info("Destroying window context");
  }

  void window_context::impl::rebuild_frame_buffers()
  {
    // new swapchain extent
    auto newWindowExtent = window_extent.load();

    // Windows: minimized window have zero extent. Wait until next event.
    while (vk::Extent2D(newWindowExtent) == vk::Extent2D(0, 0)) {
      glfwWaitEvents();
      newWindowExtent = window_extent.load();
    }

    // wait idle
    device->waitIdle();

    /* destroy swapchain resources */
    in_flight_fences.clear();
    complete_semaphores.clear();
    acquire_semaphores.clear();
    command_buffers.clear();
    frame_buffers.clear();
    swapchain_image_views.clear();
    swapchain_images.clear();

    /* reset index */
    frame_index = 0;
    image_index = 0;

    /* create new swapchain resources */

    // reload extent to get accurate value at this point
    newWindowExtent = window_extent.load();

    swapchain = createSwapchain(
      surface.get(),
      newWindowExtent,
      graphics_queue_index,
      present_queue_index,
      vulkan_ctx.physical_device(),
      device.get(),
      swapchain.get(),
      &swapchain_format,
      &swapchain_present_mode,
      &swapchain_extent,
      &swapchain_image_count);

    swapchain_images = device->getSwapchainImagesKHR(swapchain.get());

    swapchain_image_views = createSwapchainImageViews(
      swapchain.get(), swapchain_format, device.get());

    frame_buffers = createFrameBuffers(
      swapchain_image_views, render_pass.get(), swapchain_extent, device.get());

    command_buffers = createCommandBuffers(
      swapchain_image_count,
      vk::CommandBufferLevel::ePrimary,
      command_pool.get(),
      device.get());

    acquire_semaphores = createSemaphores(swapchain_image_count, device.get());

    complete_semaphores = createSemaphores(swapchain_image_count, device.get());

    in_flight_fences = createFences(
      swapchain_image_count, device.get(), vk::FenceCreateFlagBits::eSignaled);
  }

  void window_context::impl::begin_frame()
  {
    // set next frame index
    frame_index = (frame_index + 1) % swapchain_image_count;

    {
      // acquire next image, get next image_index.
      // when swapchain is out of date, rebuild it until not getting error.
      // VkAcquireNextImageKHR can be non-blocking depending on driver
      // implementation. so uses fence to prevent over-commit.

      // poll window resize event
      if (resized())
        rebuild_frame_buffers();

      // try without fence first time
      auto err = device->acquireNextImageKHR(
        swapchain.get(),
        std::numeric_limits<uint64_t>::max(),
        acquire_semaphores[frame_index].get(),
        vk::Fence(),
        &image_index);

      // loop
      while (err == vk::Result::eErrorOutOfDateKHR) {

        if (!resized())
          break; // abort

        rebuild_frame_buffers();

        device->resetFences(acquire_fence.get());

        err = device->acquireNextImageKHR(
          swapchain.get(),
          std::numeric_limits<uint64_t>::max(),
          acquire_semaphores[frame_index].get(),
          acquire_fence.get(),
          &image_index);

        auto wait_err = device->waitForFences(
          acquire_fence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());

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

      auto err = device->waitForFences(
        in_flight_fences[frame_index].get(),
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

      if (err != vk::Result::eSuccess)
        throw std::runtime_error(
          "Failed to wait for in-flight fence: " + vk::to_string(err));

      // reset fence
      device->resetFences(in_flight_fences[frame_index].get());
    }
  }

  void window_context::impl::end_frame()
  {
    std::array<vk::Semaphore, 1> waitSemaphores = {
      acquire_semaphores[frame_index].get()};
    std::array<vk::Semaphore, 1> signalSemaphores = {
      complete_semaphores[frame_index].get()};
    vk::PipelineStageFlags waitStage = {
      vk::PipelineStageFlagBits::eColorAttachmentOutput};

    /* submit current command buffers */
    {
      vk::SubmitInfo submitInfo;
      submitInfo.waitSemaphoreCount   = (uint32_t)waitSemaphores.size();
      submitInfo.pWaitSemaphores      = waitSemaphores.data();
      submitInfo.signalSemaphoreCount = (uint32_t)signalSemaphores.size();
      submitInfo.pSignalSemaphores    = signalSemaphores.data();
      submitInfo.pWaitDstStageMask    = &waitStage;
      submitInfo.commandBufferCount   = 1;
      submitInfo.pCommandBuffers      = &command_buffers[frame_index].get();

      // submit
      auto err = graphics_queue.submit(
        1, &submitInfo, in_flight_fences[frame_index].get());

      if (err != vk::Result::eSuccess)
        throw std::runtime_error(
          "Failed to submit command buffer: " + vk::to_string(err));
    }

    /* present result */
    {
      vk::PresentInfoKHR presentInfo;
      presentInfo.waitSemaphoreCount = (uint32_t)signalSemaphores.size();
      presentInfo.pWaitSemaphores    = signalSemaphores.data();
      presentInfo.swapchainCount     = 1;
      presentInfo.pSwapchains        = &swapchain.get();
      presentInfo.pImageIndices      = &image_index;

      auto err = present_queue.presentKHR(&presentInfo);

      // Surface is not longer compatible with current frame buffer.
      if (err == vk::Result::eErrorOutOfDateKHR) {
        return;
      }

      if (err != vk::Result::eSuccess)
        throw std::runtime_error("Failed to present: " + vk::to_string(err));
    }
  }

  auto window_context::impl::begin_record() -> vk::CommandBuffer
  {
    auto buffer = command_buffers[frame_index].get();

    /* begin command buffer */
    {
      vk::CommandBufferBeginInfo beginInfo;
      beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
      buffer.begin(beginInfo);
    }
    /* begin render pass */
    {
      // you can set clear color with set_clear_color()
      vk::ClearValue clearValue;
      clearValue.color = clearColor;

      // begin render pass
      vk::RenderPassBeginInfo beginInfo;
      beginInfo.renderPass        = render_pass.get();
      beginInfo.renderArea.extent = swapchain_extent;
      beginInfo.clearValueCount   = 1;
      beginInfo.pClearValues      = &clearValue;
      beginInfo.framebuffer       = frame_buffers[image_index].get();

      buffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
    }
    return buffer;
  }

  void window_context::impl::end_record(const vk::CommandBuffer& buffer)
  {
    // end render pass
    buffer.endRenderPass();
    // end command buffer
    buffer.end();
  }

  bool window_context::impl::resized() const
  {
    glfwPollEvents();
    return vk::Extent2D(window_extent) != swapchain_extent;
  }

  bool window_context::impl::should_close() const
  {
    return glfwWindowShouldClose(glfw_win.get());
  }

  void window_context::impl::set_clear_color(float r, float g, float b, float a)
  {
    clearColor = std::array {r, g, b, a};
  }

  auto window_context::impl::single_time_command() const
    -> vulkan::single_time_command
  {
    return {device.get(), graphics_queue, command_pool.get()};
  }

  window_context::window_context(vulkan_context& ctx, glfw::glfw_window& win)
    : m_pimpl {std::make_unique<impl>(ctx, win)}
  {
  }

  window_context::~window_context() noexcept = default;

  void window_context::rebuild_frame_buffers()
  {
    m_pimpl->rebuild_frame_buffers();
  }

  void window_context::begin_frame()
  {
    m_pimpl->begin_frame();
  }

  void window_context::end_frame()
  {
    m_pimpl->end_frame();
  }

  auto window_context::begin_record() -> vk::CommandBuffer
  {
    return m_pimpl->begin_record();
  }

  void window_context::end_record(const vk::CommandBuffer& buffer)
  {
    m_pimpl->end_record(buffer);
  }

  auto window_context::vulkan_ctx() -> vulkan_context&
  {
    return m_pimpl->vulkan_ctx;
  }

  auto window_context::glfw_win() -> glfw::glfw_window&
  {
    return m_pimpl->glfw_win;
  }

  auto window_context::device() const -> vk::Device
  {
    assert(m_pimpl->device);
    return m_pimpl->device.get();
  }

  auto window_context::graphics_queue_index() const -> uint32_t
  {
    return m_pimpl->graphics_queue_index;
  }

  auto window_context::present_queue_index() const -> uint32_t
  {
    return m_pimpl->present_queue_index;
  }

  auto window_context::graphics_queue() const -> vk::Queue
  {
    return m_pimpl->graphics_queue;
  }

  auto window_context::present_queue() const -> vk::Queue
  {
    return m_pimpl->present_queue;
  }

  auto window_context::surface() const -> vk::SurfaceKHR
  {
    assert(m_pimpl->surface);
    return m_pimpl->surface.get();
  }

  auto window_context::swapchain() const -> vk::SwapchainKHR
  {
    assert(m_pimpl->swapchain);
    return m_pimpl->swapchain.get();
  }

  auto window_context::swapchain_format() const -> vk::SurfaceFormatKHR
  {
    assert(m_pimpl->swapchain_format.format != vk::Format::eUndefined);
    return m_pimpl->swapchain_format;
  }

  auto window_context::swapchain_extent() const -> vk::Extent2D
  {
    return m_pimpl->swapchain_extent;
  }

  auto window_context::swapchain_images() const -> std::vector<vk::Image>
  {
    assert(!m_pimpl->swapchain_images.empty());
    return m_pimpl->swapchain_images;
  }

  auto window_context::swapchain_image_views() const
    -> std::vector<vk::ImageView>
  {
    assert(!m_pimpl->swapchain_image_views.empty());
    std::vector<vk::ImageView> ret;
    for (auto&& view : m_pimpl->swapchain_image_views) {
      ret.push_back(view.get());
    }
    return ret;
  }

  auto window_context::frame_buffers() const -> std::vector<vk::Framebuffer>
  {
    assert(!m_pimpl->frame_buffers.empty());
    std::vector<vk::Framebuffer> ret;
    for (auto&& fb : m_pimpl->frame_buffers) {
      ret.push_back(fb.get());
    }
    return ret;
  }

  auto window_context::render_pass() const -> vk::RenderPass
  {
    assert(m_pimpl->render_pass);
    return m_pimpl->render_pass.get();
  }

  auto window_context::command_pool() const -> vk::CommandPool
  {
    assert(m_pimpl->command_pool);
    return m_pimpl->command_pool.get();
  }

  auto window_context::command_buffers() const -> std::vector<vk::CommandBuffer>
  {
    assert(!m_pimpl->command_buffers.empty());
    assert(m_pimpl->command_buffers.size() == m_pimpl->swapchain_image_count);

    std::vector<vk::CommandBuffer> ret;
    for (auto&& cb : m_pimpl->command_buffers) {
      ret.push_back(cb.get());
    }
    return ret;
  }

  bool window_context::resized() const
  {
    return m_pimpl->resized();
  }

  bool window_context::should_close() const
  {
    return m_pimpl->should_close();
  }

  void window_context::set_clear_color(float r, float g, float b, float a)
  {
    m_pimpl->set_clear_color(r, g, b, a);
  }

  auto window_context::single_time_command() const
    -> vulkan::single_time_command
  {
    return m_pimpl->single_time_command();
  }

  uint32_t window_context::swapchain_index() const
  {
    return m_pimpl->image_index;
  }

  uint32_t window_context::swapchain_index_count() const
  {
    return m_pimpl->swapchain_image_count;
  }

  uint32_t window_context::frame_index() const
  {
    return m_pimpl->frame_index;
  }

  uint32_t window_context::frame_index_count() const
  {
    return m_pimpl->swapchain_image_count;
  }

} // namespace yave::vulkan