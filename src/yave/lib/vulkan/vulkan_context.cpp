//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#define GLFW_INCLUDE_VULKAN

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(vulkan)

/* Global definitions */

// Vulkan debug callback API
static PFN_vkCreateDebugReportCallbackEXT pfn_vkCreateDebugReportCallbackEXT;
// Vulkan debug callback API
static PFN_vkDestroyDebugReportCallbackEXT pfn_vkDestroyDebugReportCallbackEXT;

/// implements vkCreateDebugReportCallbackEXT
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
  VkInstance instance,
  const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
  const VkAllocationCallbacks* pAllocator,
  VkDebugReportCallbackEXT* pCallback)
{
  assert(pfn_vkCreateDebugReportCallbackEXT);
  return pfn_vkCreateDebugReportCallbackEXT(
    instance, pCreateInfo, pAllocator, pCallback);
}

// implemnts vkDestroyDebugReportCallbackEXT
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
  VkInstance instance,
  VkDebugReportCallbackEXT callback,
  const VkAllocationCallbacks* pAllocator)
{
  assert(pfn_vkDestroyDebugReportCallbackEXT);
  pfn_vkDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
}

namespace {

  /// Debug callback
  VKAPI_ATTR VkBool32 VKAPI_CALL validationCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
  {
    using namespace yave;

    (void)objectType;
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pLayerPrefix;
    (void)pUserData;

    init_logger();

    switch (flags) {
      case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
        Info(g_logger, "{}", pMessage);
        break;
      case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        Warning(g_logger, "{}", pMessage);
        break;
      case VK_DEBUG_REPORT_ERROR_BIT_EXT:
        Error(g_logger, "{}", pMessage);
        break;
    }
    return VK_FALSE;
  }

  // platform surface extensions
#if defined(VK_USE_PLATFORM_WIN32_KHR)
  //  VK_USE_PLATFORM_WIN32_KHR  - Microsoft Windows
  constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
  constexpr uint32_t PlatformSurfaceSpecVersion =
    VK_KHR_WIN32_SURFACE_SPEC_VERSION;
#else
  // VK_USE_PLATFORM_ANDROID_KHR - Android
  // VK_USE_PLATFORM_MIR_KHR     - Mir
  // VK_USE_PLATFORM_WAYLAND_KHR - Wayland
  // VK_USE_PLATFORM_XCB_KHR     - X Window System, using the XCB library
  // VK_USE_PLATFORM_XLIB_KHR    - X Window System, using the Xlib libra
  constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_SURFACE_EXTENSION_NAME;
  constexpr uint32_t PlatformSurfaceSpecVersion = VK_KHR_SURFACE_SPEC_VERSION;
#endif

  // validation layer name
  constexpr const char* ValidationLayerName =
    "VK_LAYER_LUNARG_standard_validation";

  // debug report extension
  constexpr const char* DebugReportExtensionName =
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
  constexpr uint32_t DebugReportSpecVersion = VK_EXT_DEBUG_REPORT_SPEC_VERSION;

  /// Check vk::PhysicalDeviceFeatures compatibility
  bool checkPhysicalDeviceFeatureSupport(
    const vk::PhysicalDeviceFeatures& required,
    const vk::PhysicalDeviceFeatures& supported)
  {
#define YAVE_VULKAN_DEVICE_FEATURE_TEST_H(FEATURE) \
  if (required.FEATURE && !supported.FEATURE)      \
  return false

    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(robustBufferAccess);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(fullDrawIndexUint32);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(imageCubeArray);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(independentBlend);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(geometryShader);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(tessellationShader);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sampleRateShading);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(dualSrcBlend);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(logicOp);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(multiDrawIndirect);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(drawIndirectFirstInstance);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(depthClamp);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(depthBiasClamp);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(fillModeNonSolid);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(depthBounds);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(wideLines);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(largePoints);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(alphaToOne);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(multiViewport);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(samplerAnisotropy);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(textureCompressionETC2);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(textureCompressionASTC_LDR);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(textureCompressionBC);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(occlusionQueryPrecise);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(pipelineStatisticsQuery);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(vertexPipelineStoresAndAtomics);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(fragmentStoresAndAtomics);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderTessellationAndGeometryPointSize);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderImageGatherExtended);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderStorageImageExtendedFormats);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderStorageImageMultisample);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderStorageImageReadWithoutFormat);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderStorageImageWriteWithoutFormat);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderUniformBufferArrayDynamicIndexing);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderSampledImageArrayDynamicIndexing);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderStorageBufferArrayDynamicIndexing);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderStorageImageArrayDynamicIndexing);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderClipDistance);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderCullDistance);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderFloat64);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderInt64);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderInt16);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderResourceResidency);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(shaderResourceMinLod);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sparseBinding);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sparseResidencyBuffer);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sparseResidencyImage2D);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sparseResidencyImage3D);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sparseResidency2Samples);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sparseResidency4Samples);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sparseResidency8Samples);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sparseResidency16Samples);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(sparseResidencyAliased);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(variableMultisampleRate);
    YAVE_VULKAN_DEVICE_FEATURE_TEST_H(inheritedQueries);

#undef YAVE_VULKAN_DEVICE_FEATURE_TEST_H

    return true;
  }

  /* Utility functions */

  /// create application info
  auto createApplicationInfo() -> vk::ApplicationInfo
  {
    return vk::ApplicationInfo(
      "yave::vulkan_context",
      VK_MAKE_VERSION(0, 0, 0),
      "No Engine",
      VK_MAKE_VERSION(0, 0, 0),
      VK_API_VERSION_1_1);
  }

  /// enumerate extensions
  auto getInstanceExtensions(bool enableValidationLayer)
    -> std::vector<std::string>
  {
    using namespace yave;

    // extensions required for glfw
    uint32_t n_glfwExtensions = 0;
    const char** glfwExtensions =
      glfwGetRequiredInstanceExtensions(&n_glfwExtensions);

    assert(glfwExtensions);

    // default extensions
    constexpr std::array extensions = {
      VK_KHR_SURFACE_EXTENSION_NAME, // for surface
      PlatformSurfaceExtensionName,  // for surface
    };

    Info(g_logger, "Surface spec version: {}", PlatformSurfaceSpecVersion);

    // for validation layer
    constexpr const char* debugReportExtension = DebugReportExtensionName;

    Info(g_logger, "Debug spec version: {}", DebugReportSpecVersion);

    // collect enabling extensions

    std::vector<std::string> ret;

    ret.insert(ret.end(), glfwExtensions, glfwExtensions + n_glfwExtensions);
    ret.insert(ret.end(), extensions.begin(), extensions.end());

    if (enableValidationLayer) {
      ret.push_back(debugReportExtension);
    }

    std::sort(ret.begin(), ret.end());
    auto end = std::unique(ret.begin(), ret.end());
    ret.erase(end, ret.end());

    return ret;
  }

  /// enumerate layers
  auto getInstanceLayers(bool enableValidaitonLayer) -> std::vector<std::string>
  {
    // validation layer
    constexpr const char* validationLayer = ValidationLayerName;

    std::vector<std::string> ret;

    if (enableValidaitonLayer) {
      ret.emplace_back(validationLayer);
    }

    return ret;
  }

  /// check extensions
  void checkInstanceExtensionSupport(const std::vector<std::string>& extensions)
  {
    auto supportedExtensions = vk::enumerateInstanceExtensionProperties();

    for (auto&& extension : extensions) {
      [&] {
        for (auto&& supported : supportedExtensions) {
          if (extension == supported.extensionName)
            return;
        }
        throw std::runtime_error(
          "This platform does not support required Vulkan extensions.");
      }();
    }
  }

  /// check layers
  void checkInstanceLayerSupport(const std::vector<std::string>& layers)
  {
    auto supportedLayers = vk::enumerateInstanceLayerProperties();

    for (auto&& layer : layers) {
      [&] {
        for (auto&& supported : supportedLayers) {
          if (layer == supported.layerName)
            return;
        }
        throw std::runtime_error(
          "This platform does not support required Vulkan layers.");
      }();
    }
  }

  /// load debug report symbols
  void loadDebugReportExtensions(vk::Instance instance)
  {
    assert(instance);

    pfn_vkCreateDebugReportCallbackEXT =
      (PFN_vkCreateDebugReportCallbackEXT)instance.getProcAddr(
        "vkCreateDebugReportCallbackEXT");
    pfn_vkDestroyDebugReportCallbackEXT =
      (PFN_vkDestroyDebugReportCallbackEXT)instance.getProcAddr(
        "vkDestroyDebugReportCallbackEXT");

    if (!pfn_vkCreateDebugReportCallbackEXT)
      throw std::runtime_error(
        "Failed to load function vkCreateDebugReportCallbackExt");
    if (!pfn_vkDestroyDebugReportCallbackEXT)
      throw std::runtime_error(
        "Failed to load function vkDestroyDebugReportCallbackExt");
  }

  /// create instance
  auto createInstance(bool enableValidationLayer) -> vk::UniqueInstance
  {
    using namespace yave;

    // app info
    auto appInfo = createApplicationInfo();
    // flags: none
    auto flags = vk::InstanceCreateFlags();
    // layers
    auto layers = getInstanceLayers(enableValidationLayer);
    checkInstanceLayerSupport(layers);
    // extensions
    auto extensions = getInstanceExtensions(enableValidationLayer);
    checkInstanceExtensionSupport(extensions);

    Info(g_logger, "Following instance extensions will be enabled:");
    for (auto&& e : extensions)
      Info(g_logger, " {}", e);

    Info(g_logger, "Following instance layers will be enabled:");
    for (auto&& l : layers)
      Info(g_logger, "  {}", l);

    std::vector<const char*> lNames;
    std::vector<const char*> eNames;

    lNames.reserve(layers.size());
    for (auto&& l : layers) {
      lNames.push_back(l.c_str());
    }

    eNames.reserve(extensions.size());
    for (auto&& e : extensions) {
      eNames.push_back(e.c_str());
    }

    vk::InstanceCreateInfo info;
    info.flags                   = flags;
    info.pApplicationInfo        = &appInfo;
    info.enabledLayerCount       = (uint32_t)lNames.size();
    info.ppEnabledLayerNames     = lNames.data();
    info.enabledExtensionCount   = (uint32_t)eNames.size();
    info.ppEnabledExtensionNames = eNames.data();

    // create instance
    auto instance = vk::createInstanceUnique(info);

    if (enableValidationLayer) {
      loadDebugReportExtensions(instance.get());
    }

    return instance;
  }

  /// create debug callback
  auto createDebugReportCallback(
    bool enableValidationLayer,
    vk::Instance instance) -> vk::UniqueDebugReportCallbackEXT
  {
    using namespace yave;
    assert(instance);

    // no validation
    if (!enableValidationLayer)
      return vk::UniqueDebugReportCallbackEXT();

    vk::DebugReportCallbackCreateInfoEXT info;
    // enable for error and warning
    info.setFlags(vk::DebugReportFlagsEXT(
      vk::DebugReportFlagBitsEXT::eError
      | vk::DebugReportFlagBitsEXT::eWarning));
    // set callback
    info.setPfnCallback(validationCallback);

    return instance.createDebugReportCallbackEXTUnique(info);
  }

  /// get required device types
  auto getRequiredPhysicalDeviceTypes() -> std::vector<vk::PhysicalDeviceType>
  {
    // allow any type of device
    return {vk::PhysicalDeviceType::eCpu,
            vk::PhysicalDeviceType::eDiscreteGpu,
            vk::PhysicalDeviceType::eIntegratedGpu,
            vk::PhysicalDeviceType::eOther,
            vk::PhysicalDeviceType::eVirtualGpu};
  }

  /// get required device features
  auto getRequiredPhysicalDeviceFeatures() -> vk::PhysicalDeviceFeatures
  {
    vk::PhysicalDeviceFeatures features;
    features.geometryShader = VK_TRUE;
    return features;
  }

  /// get reuqired device queue flags
  auto getRequiredPhysicalDeviceQueueFlags() -> std::vector<vk::QueueFlagBits>
  {
    return {vk::QueueFlagBits::eGraphics};
  }

  /// check physical device type
  bool physicalDeviceMeetsDeviceTypeRequirements(
    const vk::PhysicalDevice& physicalDevice)
  {
    auto required = getRequiredPhysicalDeviceTypes();
    auto property = physicalDevice.getProperties();

    return std::find(required.begin(), required.end(), property.deviceType)
           != required.end();
  }

  /// check if physical device supports required features
  bool physicalDeviceMeetsDeviceFeatureRequirements(
    const vk::PhysicalDevice& physicalDevice)
  {
    auto required  = getRequiredPhysicalDeviceFeatures();
    auto supported = physicalDevice.getFeatures();
    return checkPhysicalDeviceFeatureSupport(required, supported);
  }

  /// check if physical device support queue family requirements
  bool physicalDeviceMeetsQueueFamilyRequirements(
    const vk::PhysicalDevice& physicalDevice)
  {
    auto queueFlags = getRequiredPhysicalDeviceQueueFlags();
    auto properties = physicalDevice.getQueueFamilyProperties();

    for (auto&& flag : queueFlags) {
      auto find =
        std::find_if(properties.begin(), properties.end(), [&](auto& p) {
          return p.queueFlags & flag;
        });

      if (find == properties.end()) {
        return false;
      }
    }
    return true;
  }

  /// check if physical device is valid
  bool physicalDeviceMeetsRequirements(const vk::PhysicalDevice& physicalDevice)
  {
    return physicalDeviceMeetsDeviceTypeRequirements(physicalDevice)
           && physicalDeviceMeetsDeviceFeatureRequirements(physicalDevice)
           && physicalDeviceMeetsQueueFamilyRequirements(physicalDevice);
  }

  /// find optimal physical device
  auto getOptimalPhysicalDeviceIndex(
    const std::vector<vk::PhysicalDevice>& physicalDevices) -> uint32_t
  {
    for (uint32_t index = 0; index < physicalDevices.size(); ++index) {
      if (physicalDeviceMeetsRequirements(physicalDevices[index]))
        return index;
    }
    throw std::runtime_error("No physical device meet requirements");
  }

  /// create device
  auto acquirePhysicalDevice(vk::Instance instance) -> vk::PhysicalDevice
  {
    auto physicalDevices = instance.enumeratePhysicalDevices();

    if (physicalDevices.empty()) {
      throw std::runtime_error("No physical device avalable");
    }

    uint32_t index = getOptimalPhysicalDeviceIndex(physicalDevices);

    return physicalDevices[index];
  }

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

    Warning(g_logger, "Graphics queue does not support presentation.");

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

  void checkDeviceExtensionSupport(
    const std::vector<std::string>& extensions,
    const vk::PhysicalDevice& physicalDevice)
  {
    auto supportedExtensions =
      physicalDevice.enumerateDeviceExtensionProperties();

    for (auto&& e : extensions) {
      [&] {
        for (auto&& supported : supportedExtensions) {
          if (supported.extensionName == e)
            return;
        }
        throw std::runtime_error("Device extension not supported");
      }();
    }
  }

  void checkDeviceLayerSupport(
    const std::vector<std::string>& layers,
    const vk::PhysicalDevice& physicalDevice)
  {
    auto supportedLayers = physicalDevice.enumerateDeviceLayerProperties();

    for (auto&& l : layers) {
      [&] {
        for (auto&& supported : supportedLayers) {
          if (supported.layerName == l)
            return;
        }
        throw std::runtime_error("Device layer not supported");
      }();
    }
  }

  auto createDevice(
    uint32_t graphicsQueueIndex,
    uint32_t presentQueueIndex,
    const vk::PhysicalDevice& physicalDevice) -> vk::UniqueDevice
  {
    std::vector queueFamilyIndicies = {graphicsQueueIndex, presentQueueIndex};

    // Vulkan API requires queue family indicies to be unqiue.
    {
      std::sort(queueFamilyIndicies.begin(), queueFamilyIndicies.end());
      auto end =
        std::unique(queueFamilyIndicies.begin(), queueFamilyIndicies.end());
      queueFamilyIndicies.erase(end, queueFamilyIndicies.end());
    }

    float queuePriority = 0.f;

    std::vector<vk::DeviceQueueCreateInfo> createInfoList;
    for (auto index : queueFamilyIndicies) {
      vk::DeviceQueueCreateInfo info;
      info.flags            = vk::DeviceQueueCreateFlags();
      info.queueFamilyIndex = index;
      info.queueCount       = 1;
      info.pQueuePriorities = &queuePriority;
      createInfoList.push_back(std::move(info));
    }

    auto extensions = getDeviceExtensions();
    checkDeviceExtensionSupport(extensions, physicalDevice);

    auto layers = getDeviceLayers();
    checkDeviceLayerSupport(layers, physicalDevice);

    std::vector<const char*> eNames;
    std::vector<const char*> lNames;

    lNames.reserve(layers.size());
    for (auto&& l : layers) {
      lNames.push_back(l.c_str());
    }

    eNames.reserve(extensions.size());
    for (auto&& e : extensions) {
      eNames.push_back(e.c_str());
    }

    vk::DeviceCreateInfo info;
    info.flags                   = vk::DeviceCreateFlags();
    info.pQueueCreateInfos       = createInfoList.data();
    info.queueCreateInfoCount    = (uint32_t)createInfoList.size();
    info.ppEnabledLayerNames     = lNames.data();
    info.enabledLayerCount       = (uint32_t)lNames.size();
    info.ppEnabledExtensionNames = eNames.data();
    info.enabledExtensionCount   = (uint32_t)eNames.size();
    info.pEnabledFeatures = nullptr; // TODO: enable only reauired features.

    return physicalDevice.createDeviceUnique(info);
  }

  auto getDeviceQueue(uint32_t queueFamilyIndex, const vk::Device& device)
    -> vk::Queue
  {
    // Assume only single queue is initialized.
    return device.getQueue(queueFamilyIndex, 0);
  }

} // namespace

namespace yave::vulkan {

  // -----------------------------------------
  // vulkan_context

  class vulkan_context::impl
  {
  public:
    impl() = default;

    // glfw
    glfw::glfw_context* glfw;

    /* instance */

    /// instance
    vk::UniqueInstance instance;
    /// validation callback
    vk::UniqueDebugReportCallbackEXT debugCallback;

    /* physical device */

    /// physical device
    vk::PhysicalDevice physicalDevice;
    /// property of physical device
    vk::PhysicalDeviceProperties physicalDeviceProperties;
    /// queue family property of physical device
    std::vector<vk::QueueFamilyProperties> physicalDeviceQueueFamilyProperties;

    /* device queue */

    /// index of graphics queue
    uint32_t graphicsQueueIndex;
    /// graphics queue
    vk::Queue graphicsQueue;
    /// index of present queue
    uint32_t presentQueueIndex;
    /// present queue
    vk::Queue presentQueue;

    /* logical device */

    /// device
    vk::UniqueDevice device;
  };

  auto vulkan_context::_init_flags() noexcept -> init_flags
  {
    return init_flags::enable_validation | init_flags::enable_logging;
  }

  vulkan_context::vulkan_context(
    [[maybe_unused]] glfw::glfw_context& glfw_ctx,
    init_flags flags)
  {
    init_logger();

    if (!glfwVulkanSupported())
      throw std::runtime_error("GLFW could not find Vulkan");

    if (!(flags & init_flags::enable_logging))
      g_logger->set_level(spdlog::level::off);

    Info(g_logger, "Initializing vulkan_context");

    /// pimpl
    auto impl = std::make_unique<vulkan_context::impl>();

    // glfw
    impl->glfw = &glfw_ctx;

    /* instance */

    bool enableValidationLayer = !!(flags & init_flags::enable_validation);

    impl->instance = createInstance(enableValidationLayer);
    impl->debugCallback =
      createDebugReportCallback(enableValidationLayer, impl->instance.get());

    /* physical device */

    impl->physicalDevice = acquirePhysicalDevice(impl->instance.get());
    impl->physicalDeviceProperties = impl->physicalDevice.getProperties();
    impl->physicalDeviceQueueFamilyProperties =
      impl->physicalDevice.getQueueFamilyProperties();

    /* device queue settings */

    impl->graphicsQueueIndex = getGraphicsQueueIndex(impl->physicalDevice);

    Info(g_logger, "Graphs queue idex: {}", impl->graphicsQueueIndex);

    impl->presentQueueIndex = getPresentQueueIndex(
      impl->graphicsQueueIndex, impl->instance.get(), impl->physicalDevice);

    Info(g_logger, "Presentation queue index: {}", impl->presentQueueIndex);

    /* logical device */

    impl->device = createDevice(
      impl->graphicsQueueIndex, impl->presentQueueIndex, impl->physicalDevice);

    Info(g_logger, "Created new logical device");

    /* get queue */

    impl->graphicsQueue =
      getDeviceQueue(impl->graphicsQueueIndex, impl->device.get());
    impl->presentQueue =
      getDeviceQueue(impl->presentQueueIndex, impl->device.get());

    if (impl->graphicsQueueIndex == impl->presentQueueIndex) {
      assert(impl->graphicsQueue == impl->presentQueue);
    }

    m_pimpl = std::move(impl);
    Info(g_logger, "Initialized Vulkan context");
  }

  vulkan_context::~vulkan_context() noexcept
  {
    Info(g_logger, "Destroying Vulkan context");
  }

  auto vulkan_context::instance() const -> vk::Instance
  {
    return m_pimpl->instance.get();
  }

  auto vulkan_context::physical_device() const -> vk::PhysicalDevice
  {
    return m_pimpl->physicalDevice;
  }

  auto vulkan_context::device() const -> vk::Device
  {
    return m_pimpl->device.get();
  }

  auto vulkan_context::graphics_queue_family_index() const -> uint32_t
  {
    return m_pimpl->graphicsQueueIndex;
  }

  auto vulkan_context::graphics_queue() const -> vk::Queue
  {
    return m_pimpl->graphicsQueue;
  }

  auto vulkan_context::present_queue_family_index() const -> uint32_t
  {
    return m_pimpl->presentQueueIndex;
  }

  auto vulkan_context::present_queue() const -> vk::Queue
  {
    return m_pimpl->presentQueue;
  }

} // namespace yave::vulkan