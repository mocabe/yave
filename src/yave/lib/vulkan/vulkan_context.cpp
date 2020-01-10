//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#define _ENABLE_ATOMIC_ALIGNMENT_FIX
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
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&n_glfwExtensions);

    if (!glfwExtensions)
      Warning(g_logger, "glfwGetRequiredInstanceExtensions() retuend NULL.");

    // default extensions
    static constexpr const char* extensions[] = {
      VK_KHR_SURFACE_EXTENSION_NAME, // for surface
      PlatformSurfaceExtensionName,  // for surface
    };

    Info(g_logger, "Surface spec version: {}", PlatformSurfaceSpecVersion);

    // for validation layer
    static constexpr const char* debugReportExtension =
      DebugReportExtensionName;

    Info(g_logger, "Debug spec version: {}", DebugReportSpecVersion);

    std::vector<std::string> ret;

    for (size_t i = 0; i < n_glfwExtensions; ++i) {
      ret.emplace_back(glfwExtensions[i]);
    }
    for (auto&& e : extensions) {
      ret.emplace_back(e);
    }
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
    static constexpr const char* validationLayer = ValidationLayerName;

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
    uint32_t index = 0;
    for (; index < physicalDevices.size(); ++index) {
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
    using namespace yave;

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
    static constexpr const char* extensions[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, // for swapchain
    };

    std::vector<std::string> ret;
    for (auto&& e : extensions) {
      ret.emplace_back(e);
    }
    return ret;
  }

  auto getDeviceLayers() -> std::vector<std::string>
  {
    return {};
  }

  auto createDevice(
    uint32_t graphicsQueueIndex,
    uint32_t presentQueueIndex,
    const vk::PhysicalDevice& physicalDevice) -> vk::UniqueDevice
  {
    std::vector<uint32_t> queueFamilyIndicies = {graphicsQueueIndex,
                                                 presentQueueIndex};

    // Vulkan API requires queue family indicies to be unqiue.
    std::sort(queueFamilyIndicies.begin(), queueFamilyIndicies.end());
    auto unique_end =
      std::unique(queueFamilyIndicies.begin(), queueFamilyIndicies.end());
    queueFamilyIndicies.erase(unique_end, queueFamilyIndicies.end());

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
    auto layers     = getDeviceLayers();

    auto supportedExtensions =
      physicalDevice.enumerateDeviceExtensionProperties();
    auto supportedLayers = physicalDevice.enumerateDeviceLayerProperties();

    for (auto&& e : extensions) {
      [&] {
        for (auto&& supported : supportedExtensions) {
          if (supported.extensionName == e)
            return;
        }
        throw std::runtime_error("Device extension not supported");
      }();
    }

    for (auto&& l : layers) {
      [&] {
        for (auto&& supported : supportedLayers) {
          if (supported.layerName == l)
            return;
        }
        throw std::runtime_error("Device layer not supported");
      }();
    }

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

    Warning(
      g_logger,
      "Default surface format is not avalable. Return first format avalable.");

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
    using namespace yave;

    if (
      capabilities.supportedCompositeAlpha
      & vk::CompositeAlphaFlagBitsKHR::eOpaque)
      return vk::CompositeAlphaFlagBitsKHR::eOpaque;

    Warning(
      g_logger,
      "vk::CompositeAlphaFlagBitsKHR::eOpaque is not supported. Use eInherit");

    return vk::CompositeAlphaFlagBitsKHR::eInherit;
  }

  auto createSwapchain(
    const vk::SurfaceKHR& surface,
    const vk::Extent2D windowExtent,
    uint32_t graphicsQueueIndex,
    uint32_t presentQueueIndex,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& logicalDevice,
    vk::SurfaceFormatKHR* out_format     = nullptr,
    vk::PresentModeKHR* out_present_mode = nullptr,
    vk::Extent2D* out_extent             = nullptr,
    uint32_t* out_image_count            = nullptr) -> vk::UniqueSwapchainKHR
  {
    if (!physicalDevice.getSurfaceSupportKHR(presentQueueIndex, surface)) {
      throw std::runtime_error(
        "Current surface format is not supported by presentation queue family");
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

    auto preTransform = chooseSwapchainPreTransform(capabilities);

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
    // single layer
    info.imageArrayLayers = 1;
    // directly render (as color attachment)
    info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    uint32_t queueFamilyIndicies[] = {graphicsQueueIndex, presentQueueIndex};

    if (graphicsQueueIndex == presentQueueIndex) {
      info.imageSharingMode      = vk::SharingMode::eExclusive;
      info.queueFamilyIndexCount = 0;
      info.pQueueFamilyIndices   = nullptr;
    } else {
      info.imageSharingMode      = vk::SharingMode::eConcurrent;
      info.queueFamilyIndexCount = 2;
      info.pQueueFamilyIndices   = queueFamilyIndicies;
    }

    // create swapchain

    auto swapchain = logicalDevice.createSwapchainKHRUnique(info);

    // write out pointers

    if (out_format)
      *out_format = format;
    if (out_present_mode)
      *out_present_mode = mode;
    if (out_extent)
      *out_extent = extent;
    if (out_image_count)
      *out_image_count = imageCount;

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
    for (uint32_t i = 0; i < size; ++i) {
      ret.push_back(createFence(device, flags));
    }
    return ret;
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

  vulkan_context::vulkan_context(
    [[maybe_unused]] glfw::glfw_context& glfw_ctx,
    bool enableValidationLayer)
  {
    init_logger();

    if (!glfwVulkanSupported())
      throw std::runtime_error("GLFW could not find Vulkan");

    Info(g_logger, "Initializing vulkan_context");

    /// pimpl
    auto impl = std::make_unique<vulkan_context::impl>();

    // glfw
    impl->glfw = &glfw_ctx;

    /* instance */

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

  // -----------------------------------------
  // window_context

  class window_context::impl
  {
  public:
    impl() = default;
    vk::UniqueSurfaceKHR surface;
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
    const vulkan_context* context; // non-owning
    GLFWwindow* window;            // non-owning
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
  };

  auto vulkan_context::create_window_context(glfw::glfw_window& window) const
    -> window_context
  {
    Info(g_logger, "Initializing window context");

    auto impl = std::make_unique<window_context::impl>();

    // back pointer
    impl->context = this;
    // window
    impl->window = window.get();

    // get window size
    impl->window_extent = getWindowExtent(window.get());

    // key for user data pointer of window context
    static constexpr const char* wd_key = "widnow_context";

    // set user data
    if (!glfw::glfw_window::add_user_data(window.get(), wd_key, impl.get())) {
      throw std::runtime_error("Failed to set window data");
    }

    // set window resize callback
    glfwSetFramebufferSizeCallback(
      window.get(), [](GLFWwindow* window, int width, int height) noexcept {
        if (
          auto* ctx = (window_context::impl*)glfw::glfw_window::get_user_data(
            window, wd_key)) {
          // set new window size
          ctx->window_extent = {
            static_cast<uint32_t>(std::clamp(0, width, width)),
            static_cast<uint32_t>(std::clamp(0, height, height))};
        }
      });

    // create surface
    impl->surface = createWindowSurface(window.get(), m_pimpl->instance.get());

    Info(g_logger, "Created new surface");

    // create swapchain
    impl->swapchain = createSwapchain(
      impl->surface.get(),
      {impl->window_extent},
      m_pimpl->graphicsQueueIndex,
      m_pimpl->presentQueueIndex,
      m_pimpl->physicalDevice,
      m_pimpl->device.get(),
      &impl->swapchain_format,       // out
      &impl->swapchain_present_mode, // out
      &impl->swapchain_extent,       // out
      &impl->swapchain_image_count); // out

    Info(g_logger, "Created new swapchain");

    // get swapchain images
    impl->swapchain_images =
      m_pimpl->device->getSwapchainImagesKHR(impl->swapchain.get());

    // create image views
    impl->swapchain_image_views = createSwapchainImageViews(
      impl->swapchain.get(), impl->swapchain_format, m_pimpl->device.get());

    assert(impl->swapchain_images.size() == impl->swapchain_image_views.size());
    assert(impl->swapchain_images.size() == impl->swapchain_image_count);

    Info(g_logger, "Created swapchain image views");

    // create render pass
    impl->render_pass =
      createRenderPass(impl->swapchain_format, m_pimpl->device.get());

    Info(g_logger, "Created render pass");

    // create frame buffers
    impl->frame_buffers = createFrameBuffers(
      impl->swapchain_image_views,
      impl->render_pass.get(),
      impl->swapchain_extent,
      m_pimpl->device.get());

    Info(g_logger, "Created frame buffers");

    // create command pool
    impl->command_pool =
      createCommandPool(m_pimpl->graphicsQueueIndex, m_pimpl->device.get());

    Info(g_logger, "Created command pool");

    // create command buffers
    impl->command_buffers = createCommandBuffers(
      impl->swapchain_image_count,
      vk::CommandBufferLevel::ePrimary,
      impl->command_pool.get(),
      m_pimpl->device.get());

    assert(impl->command_buffers.size() == impl->frame_buffers.size());

    Info(g_logger, "Created command buffers");

    /* semaphores */

    impl->acquire_semaphores =
      createSemaphores(impl->swapchain_image_count, m_pimpl->device.get());
    impl->complete_semaphores =
      createSemaphores(impl->swapchain_image_count, m_pimpl->device.get());

    Info(g_logger, "Created semaphores");

    /* fences */

    // create with signaled state
    impl->in_flight_fences = createFences(
      impl->swapchain_image_count,
      m_pimpl->device.get(),
      vk::FenceCreateFlagBits::eSignaled);

    impl->acquire_fence =
      createFence(m_pimpl->device.get(), vk::FenceCreateFlags());

    Info(g_logger, "Created fences");

    window_context ctx;
    ctx.m_pimpl = std::move(impl);

    Info(g_logger, "Initialized new window context");

    return ctx;
  }

  void window_context::rebuild_frame_buffers()
  {
    // new swapchain extent
    auto newWindowExtent = m_pimpl->window_extent.load();

    // Windows: minimized window have zero extent. Wait until next event.
    while (vk::Extent2D(newWindowExtent) == vk::Extent2D(0, 0)) {
      glfwWaitEvents();
      newWindowExtent = m_pimpl->window_extent.load();
    }

    // wait idle
    m_pimpl->context->device().waitIdle();

    /* destroy swapchain resources */
    m_pimpl->in_flight_fences.clear();
    m_pimpl->complete_semaphores.clear();
    m_pimpl->acquire_semaphores.clear();
    m_pimpl->command_buffers.clear();
    m_pimpl->frame_buffers.clear();
    m_pimpl->render_pass.reset();
    m_pimpl->swapchain_image_views.clear();
    m_pimpl->swapchain_images.clear();
    m_pimpl->swapchain.reset();

    /* reset index */
    m_pimpl->frame_index = 0;
    m_pimpl->image_index = 0;

    /* create new swapchain resources */

    // reload extent to get accurate value at this point
    newWindowExtent = m_pimpl->window_extent.load();

    m_pimpl->swapchain = createSwapchain(
      m_pimpl->surface.get(),
      newWindowExtent,
      m_pimpl->context->graphics_queue_family_index(),
      m_pimpl->context->present_queue_family_index(),
      m_pimpl->context->physical_device(),
      m_pimpl->context->device(),
      &m_pimpl->swapchain_format,
      &m_pimpl->swapchain_present_mode,
      &m_pimpl->swapchain_extent,
      &m_pimpl->swapchain_image_count);

    m_pimpl->swapchain_images =
      m_pimpl->context->device().getSwapchainImagesKHR(
        m_pimpl->swapchain.get());

    m_pimpl->swapchain_image_views = createSwapchainImageViews(
      m_pimpl->swapchain.get(),
      m_pimpl->swapchain_format,
      m_pimpl->context->device());

    m_pimpl->render_pass =
      createRenderPass(m_pimpl->swapchain_format, m_pimpl->context->device());

    m_pimpl->frame_buffers = createFrameBuffers(
      m_pimpl->swapchain_image_views,
      m_pimpl->render_pass.get(),
      m_pimpl->swapchain_extent,
      m_pimpl->context->device());

    m_pimpl->command_buffers = createCommandBuffers(
      m_pimpl->swapchain_image_count,
      vk::CommandBufferLevel::ePrimary,
      m_pimpl->command_pool.get(),
      m_pimpl->context->device());

    m_pimpl->acquire_semaphores = createSemaphores(
      m_pimpl->swapchain_image_count, m_pimpl->context->device());

    m_pimpl->complete_semaphores = createSemaphores(
      m_pimpl->swapchain_image_count, m_pimpl->context->device());

    m_pimpl->in_flight_fences = createFences(
      m_pimpl->swapchain_image_count,
      m_pimpl->context->device(),
      vk::FenceCreateFlagBits::eSignaled);
  }

  void window_context::begin_frame()
  {
    auto device = m_pimpl->context->device();

    // set next frame index
    m_pimpl->frame_index =
      (m_pimpl->frame_index + 1) % m_pimpl->swapchain_image_count;

    {
      // acquire next image, get next image_index.
      // when swapchain is out of date, rebuild it until not getting error.
      // VkAcquireNextImageKHR can be non-blocking depending on driver
      // implementation. so uses fence to prevent over-commit.

      // poll window resize event
      if (resized())
        rebuild_frame_buffers();

      // try without fence first time
      auto err = device.acquireNextImageKHR(
        m_pimpl->swapchain.get(),
        std::numeric_limits<uint64_t>::max(),
        m_pimpl->acquire_semaphores[m_pimpl->frame_index].get(),
        vk::Fence(),
        &m_pimpl->image_index);

      // loop
      while (err == vk::Result::eErrorOutOfDateKHR) {

        if (!resized())
          break; // abort

        rebuild_frame_buffers();

        device.resetFences(m_pimpl->acquire_fence.get());

        err = device.acquireNextImageKHR(
          m_pimpl->swapchain.get(),
          std::numeric_limits<uint64_t>::max(),
          m_pimpl->acquire_semaphores[m_pimpl->frame_index].get(),
          m_pimpl->acquire_fence.get(),
          &m_pimpl->image_index);

        auto wait_err = device.waitForFences(
          m_pimpl->acquire_fence.get(),
          VK_TRUE,
          std::numeric_limits<uint64_t>::max());

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
        m_pimpl->in_flight_fences[m_pimpl->frame_index].get(),
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

      if (err != vk::Result::eSuccess)
        throw std::runtime_error(
          "Failed to wait for in-flight fence: " + vk::to_string(err));

      // reset fence
      device.resetFences(m_pimpl->in_flight_fences[m_pimpl->frame_index].get());
    }
  }

  void window_context::end_frame() const
  {
    auto graphicsQueue = m_pimpl->context->graphics_queue();
    auto presentQueue  = m_pimpl->context->present_queue();

    std::array<vk::Semaphore, 1> waitSemaphores = {
      m_pimpl->acquire_semaphores[m_pimpl->frame_index].get()};
    std::array<vk::Semaphore, 1> signalSemaphores = {
      m_pimpl->complete_semaphores[m_pimpl->frame_index].get()};
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
      submitInfo.pCommandBuffers =
        &m_pimpl->command_buffers[m_pimpl->frame_index].get();

      // submit
      auto err = graphicsQueue.submit(
        1, &submitInfo, m_pimpl->in_flight_fences[m_pimpl->frame_index].get());

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
      presentInfo.pSwapchains        = &m_pimpl->swapchain.get();
      presentInfo.pImageIndices      = &m_pimpl->image_index;

      auto err = presentQueue.presentKHR(&presentInfo);

      // Surface is not longer compatible with current frame buffer.
      if (err == vk::Result::eErrorOutOfDateKHR) {
        return;
      }

      if (err != vk::Result::eSuccess)
        throw std::runtime_error("Failed to present: " + vk::to_string(err));
    }
  }

  auto window_context::begin_record() const -> vk::CommandBuffer
  {
    auto buffer = m_pimpl->command_buffers[m_pimpl->frame_index].get();

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
      clearValue.color = m_pimpl->clearColor;

      // begin render pass
      vk::RenderPassBeginInfo beginInfo;
      beginInfo.renderPass        = m_pimpl->render_pass.get();
      beginInfo.renderArea.extent = m_pimpl->swapchain_extent;
      beginInfo.clearValueCount   = 1;
      beginInfo.pClearValues      = &clearValue;
      beginInfo.framebuffer =
        m_pimpl->frame_buffers[m_pimpl->image_index].get();

      buffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
    }
    return buffer;
  }

  void window_context::end_record(const vk::CommandBuffer& buffer) const
  {
    // end render pass
    buffer.endRenderPass();
    // end command buffer
    buffer.end();
  }

  window_context::window_context()
  {
    // m_pimpl will be initialized by vulkan_context.
  }

  window_context::window_context(window_context&& other) noexcept
    : m_pimpl {std::move(other.m_pimpl)}
  {
  }

  window_context::~window_context() noexcept
  {
    using namespace yave;

    if (!m_pimpl)
      return;

    m_pimpl->context->device().waitIdle();
    Info(g_logger, "Destroying window context");
  }

  auto window_context::window() const -> GLFWwindow*
  {
    assert(m_pimpl->window);
    return m_pimpl->window;
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
    glfwPollEvents();
    return vk::Extent2D(m_pimpl->window_extent) != m_pimpl->swapchain_extent;
  }

  bool window_context::should_close() const
  {
    return glfwWindowShouldClose(m_pimpl->window);
  }

  void window_context::set_clear_color(float r, float g, float b, float a)
  {
    m_pimpl->clearColor = std::array {r, g, b, a};
  }

  auto window_context::single_time_command() const
    -> vulkan::single_time_command
  {
    return {m_pimpl->context->device(),
            m_pimpl->context->graphics_queue(),
            m_pimpl->command_pool.get()};
  }

  auto window_context::new_recorder() -> command_recorder
  {
    return command_recorder(this);
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

  // -----------------------------------------
  // command_recorder

  command_recorder::command_recorder(window_context* window_ctx)
    : m_window_ctx {window_ctx}
  {
    m_window_ctx->begin_frame();
    m_buffer = m_window_ctx->begin_record();
  }

  command_recorder::~command_recorder()
  {
    if (!m_window_ctx)
      return;

    m_window_ctx->end_record(m_buffer);
    m_window_ctx->end_frame();
  }

  command_recorder::command_recorder(command_recorder&& other) noexcept
    : m_window_ctx {std::move(other.m_window_ctx)}
    , m_buffer {std::move(other.m_buffer)}
  {
  }

  auto command_recorder::command_buffer() const -> vk::CommandBuffer
  {
    return m_buffer;
  }

} // namespace yave::vulkan