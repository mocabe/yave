//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/support/vulkan_context.hpp>
#include <yave/support/log.hpp>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

/* Global definitions */

namespace {
  // Vulkan debug callback API
  PFN_vkCreateDebugReportCallbackEXT pfn_vkCreateDebugReportCallbackEXT;
  // Vulkan debug callback API
  PFN_vkDestroyDebugReportCallbackEXT pfn_vkDestroyDebugReportCallbackEXT;
} // namespace

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

  // logger
  std::shared_ptr<spdlog::logger> g_vulkan_logger;

  // init
  void init_vulkan_logger()
  {
    [[maybe_unused]] static auto init_logger = [] {
      g_vulkan_logger = yave::add_logger("vulkan_context");
      return 1;
    }();
  }

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

    init_vulkan_logger();

    switch (flags) {
      case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
        Info(g_vulkan_logger, "{}", pMessage);
        break;
      case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        Warning(g_vulkan_logger, "{}", pMessage);
        break;
      case VK_DEBUG_REPORT_ERROR_BIT_EXT:
        Error(g_vulkan_logger, "{}", pMessage);
        break;
    }
    return VK_FALSE;
  }

  // platform surface extensions
#if defined(VK_USE_PLATFORM_WIN32_KHR)
  //  VK_USE_PLATFORM_WIN32_KHR  - Microsoft Windows
  static constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
  static constexpr uint32_t PlatformSurfaceSpecVersion =
    VK_KHR_WIN32_SURFACE_SPEC_VERSION;
#else
  // VK_USE_PLATFORM_ANDROID_KHR - Android
  // VK_USE_PLATFORM_MIR_KHR     - Mir
  // VK_USE_PLATFORM_WAYLAND_KHR - Wayland
  // VK_USE_PLATFORM_XCB_KHR     - X Window System, using the XCB library
  // VK_USE_PLATFORM_XLIB_KHR    - X Window System, using the Xlib libra
  static constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_SURFACE_EXTENSION_NAME;
  static constexpr uint32_t PlatformSurfaceSpecVersion =
    VK_KHR_SURFACE_SPEC_VERSION;
#endif

  // validation layer name
  static constexpr const char* ValidationLayerName =
    "VK_LAYER_LUNARG_standard_validation";

  // debug report extension
  static constexpr const char* DebugReportExtensionName =
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
  static constexpr uint32_t DebugReportSpecVersion =
    VK_EXT_DEBUG_REPORT_SPEC_VERSION;

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
  vk::ApplicationInfo createApplicationInfo()
  {
    return vk::ApplicationInfo(
      "yave::vulkan_context",
      VK_MAKE_VERSION(0, 0, 0),
      "No Engine",
      VK_MAKE_VERSION(0, 0, 0),
      VK_API_VERSION_1_1);
  }

  /// enumerate extensions
  std::vector<std::string> getInstanceExtensions(bool enableValidationLayer)
  {
    using namespace yave;

    // extensions required for glfw
    uint32_t n_glfwExtensions = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&n_glfwExtensions);

    if (!glfwExtensions)
      Warning(
        g_vulkan_logger, "glfwGetRequiredInstanceExtensions() retuend NULL.");

    // default extensions
    static constexpr const char* extensions[] = {
      VK_KHR_SURFACE_EXTENSION_NAME,   // for surface
      PlatformSurfaceExtensionName,    // for surface
    };

    // for validation layer
    static constexpr const char* debugReportExtension =
      DebugReportExtensionName;

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
  std::vector<std::string> getInstanceLayers(bool enableValidaitonLayer)
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
  vk::UniqueInstance createInstance(bool enableValidationLayer)
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

    Info(g_vulkan_logger, "Following instance extensions will be enabled:");
    for (auto&& e : extensions) Info(g_vulkan_logger, " {}", e);

    Info(g_vulkan_logger, "Following instance layers will be enabled:");
    for (auto&& l : layers) Info(g_vulkan_logger, "  {}", l);

    std::vector<const char*> lNames;
    std::vector<const char*> eNames;

    for (auto&& l : layers) {
      lNames.push_back(l.c_str());
    }
    for (auto&& e : extensions) {
      eNames.push_back(e.c_str());
    }

    vk::InstanceCreateInfo info;
    info.flags                   = flags;
    info.pApplicationInfo        = &appInfo;
    info.enabledLayerCount       = lNames.size();
    info.ppEnabledLayerNames     = lNames.data();
    info.enabledExtensionCount   = eNames.size();
    info.ppEnabledExtensionNames = eNames.data();

    // create instance
    auto instance = vk::createInstanceUnique(info);

    if (enableValidationLayer) {
      loadDebugReportExtensions(instance.get());
    }

    Info(g_vulkan_logger, "Created Vulkan instance");

    return instance;
  }

  /// create debug callback
  vk::UniqueDebugReportCallbackEXT
    createDebugReportCallback(bool enableValidationLayer, vk::Instance instance)
  {
    using namespace yave;
    assert(instance);

    // no validation
    if (!enableValidationLayer)
      return vk::UniqueDebugReportCallbackEXT();

    vk::DebugReportCallbackCreateInfoEXT info;
    // enable for error and warning
    info.setFlags(vk::DebugReportFlagsEXT(
      vk::DebugReportFlagBitsEXT::eError |
      vk::DebugReportFlagBitsEXT::eWarning));
    // set callback
    info.setPfnCallback(validationCallback);

    Info(g_vulkan_logger, "Created debug report callback");

    return instance.createDebugReportCallbackEXTUnique(info);
  }

  /// get required device types
  std::vector<vk::PhysicalDeviceType> getRequiredPhysicalDeviceTypes()
  {
    // allow any type of device
    return {vk::PhysicalDeviceType::eCpu,
            vk::PhysicalDeviceType::eDiscreteGpu,
            vk::PhysicalDeviceType::eIntegratedGpu,
            vk::PhysicalDeviceType::eOther,
            vk::PhysicalDeviceType::eVirtualGpu};
  }

  /// get required device features
  vk::PhysicalDeviceFeatures getRequiredPhysicalDeviceFeatures()
  {
    vk::PhysicalDeviceFeatures features;
    features.geometryShader = VK_TRUE;
    return features;
  }

  /// get reuqired device queue flags
  std::vector<vk::QueueFlagBits> getRequiredPhysicalDeviceQueueFlags()
  {
    return {vk::QueueFlagBits::eGraphics};
  }

  /// check physical device type
  bool physicalDeviceMeetsDeviceTypeRequirements(
    const vk::PhysicalDevice& physicalDevice)
  {
    auto required = getRequiredPhysicalDeviceTypes();
    auto property = physicalDevice.getProperties();

    return std::find(required.begin(), required.end(), property.deviceType) !=
           required.end();
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
    return physicalDeviceMeetsDeviceTypeRequirements(physicalDevice) &&
           physicalDeviceMeetsDeviceFeatureRequirements(physicalDevice) &&
           physicalDeviceMeetsQueueFamilyRequirements(physicalDevice);
  }

  /// find optimal physical device
  uint32_t getOptimalPhysicalDeviceIndex(
    const std::vector<vk::PhysicalDevice>& physicalDevices)
  {
    uint32_t index = 0;
    for (; index < physicalDevices.size(); ++index) {
      if (physicalDeviceMeetsRequirements(physicalDevices[index]))
        return index;
    }
    throw std::runtime_error("No physical device meet requirements");
  }

  /// create device
  vk::PhysicalDevice acquirePhysicalDevice(vk::Instance instance)
  {
    using namespace yave;
    assert(instance);

    auto physicalDevices = instance.enumeratePhysicalDevices();

    Info(
      g_vulkan_logger,
      "{} physical devices are installed:",
      physicalDevices.size());
    for (auto&& d : physicalDevices) {
      auto property = d.getProperties();
      Info(
        g_vulkan_logger, "  [{}]: {}", property.deviceID, property.deviceName);
    }

    if (physicalDevices.empty()) {
      throw std::runtime_error("No physical device avalable");
    }

    uint32_t index = getOptimalPhysicalDeviceIndex(physicalDevices);

    Info(
      g_vulkan_logger,
      "Use physical device #{}: {}",
      index,
      physicalDevices[index].getProperties().deviceName);

    Info(
      g_vulkan_logger,
      "Queue family information of physical device {}",
      physicalDevices[index].getProperties().deviceName);
    for (auto&& prop : physicalDevices[index].getQueueFamilyProperties()) {
      Info(
        g_vulkan_logger,
        "  ({}) {}",
        prop.queueCount,
        vk::to_string(prop.queueFlags));
    }

    return physicalDevices[index];
  }

  uint32_t getGraphicsQueueIndex(
    const vk::PhysicalDevice& physicalDevice)
  {
    using namespace yave;

    auto properties = physicalDevice.getQueueFamilyProperties();

    for (size_t i = 0; i < properties.size(); ++i) {
      if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics)
        return i;
    }

    throw std::runtime_error("Device does not support graphics queue");
  }

  uint32_t getPresentQueueIndex(
    uint32_t graphicsQueueIndex,
    const vk::Instance& instance,
    const vk::PhysicalDevice& physicalDevice)
  {
    using namespace yave;

    // when graphics queue supports presentation, use graphics queue.
    if (glfwGetPhysicalDevicePresentationSupport(
          instance, physicalDevice, graphicsQueueIndex)) {
      return graphicsQueueIndex;
    }

    Warning(
      g_vulkan_logger,
      "Graphics queue does not support presentation. Try to find from other "
      "queue families");

    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
      auto support =
        glfwGetPhysicalDevicePresentationSupport(instance, physicalDevice, i);

      Info(
        g_vulkan_logger, "QueueFamily[{}]: presentationSupport={}", i, support);

      if (support)
        return i;
    }

    throw std::runtime_error("Presentation is not supported on this device");
  }

  std::vector<uint32_t>
    getUniqueQueueFamilyIndicies(std::vector<uint32_t> indicies)
  {
    std::sort(indicies.begin(), indicies.end());
    auto iter = std::unique(indicies.begin(), indicies.end());
    indicies.erase(iter, indicies.end());
    return indicies;
  }

  std::vector<std::string> getDeviceExtensions()
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

  std::vector<std::string> getDeviceLayers()
  {
    return {};
  }

  vk::UniqueDevice createDevice(
    const std::vector<uint32_t>& queueFamilyIndicies,
    const vk::PhysicalDevice& physicalDevice)
  {
    using namespace yave;

    std::vector<vk::DeviceQueueCreateInfo> createInfoList;
    for (auto index : queueFamilyIndicies) {
      vk::DeviceQueueCreateInfo info;
      info.flags            = vk::DeviceQueueCreateFlags();
      info.queueFamilyIndex = index;
      info.queueCount       = 1;
      float queuePriority   = 0.f;
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

    for (auto&& e : extensions) {
      eNames.push_back(e.c_str());
    }
    for (auto&& l : layers) {
      lNames.push_back(l.c_str());
    }

    vk::DeviceCreateInfo info;
    info.flags                   = vk::DeviceCreateFlags();
    info.pQueueCreateInfos       = createInfoList.data();
    info.queueCreateInfoCount    = createInfoList.size();
    info.ppEnabledLayerNames     = lNames.data();
    info.enabledLayerCount       = lNames.size();
    info.ppEnabledExtensionNames = eNames.data();
    info.enabledExtensionCount   = eNames.size();
    info.pEnabledFeatures = nullptr; // TODO: enable only reauired features.

    auto device = physicalDevice.createDeviceUnique(info);

    Info(g_vulkan_logger, "Created logical device");

    return device;
  }

  vk::SurfaceFormatKHR
    chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
  {
    using namespace yave;

    auto default_format = vk::SurfaceFormatKHR {
      vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};

    Info(
      g_vulkan_logger,
      "Default surface format: {}/{}",
      vk::to_string(default_format.format),
      vk::to_string(default_format.colorSpace));

    if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
      Info(
        g_vulkan_logger,
        "Surface format is undefined. Use default format");
      return default_format;
    }

    for (auto&& format : formats) {
      if (format == default_format)
        Info(g_vulkan_logger, "Default surface format is supported");
      return default_format;
    }

    Info(
      g_vulkan_logger,
      "Default surface format is not supported. "
      "Use first format avalable.");

    return formats[0];
  }

  vk::PresentModeKHR
    choosePresentMode(const std::vector<vk::PresentModeKHR>& modes)
  {
    for (auto&& mode : modes) {
      if (mode == vk::PresentModeKHR::eMailbox)
        return mode;
      if (mode == vk::PresentModeKHR::eImmediate)
        return mode;
    }
    return vk::PresentModeKHR::eFifo;
  }

  vk::Extent2D chooseSwapchainExtent(
    vk::Extent2D window_size,
    const vk::SurfaceCapabilitiesKHR& capabilities)
  {
    using namespace yave;

    if (
      capabilities.currentExtent.width ==
      std::numeric_limits<uint32_t>::max()) {

      Warning(g_vulkan_logger, "currentExtend is not set. Use clamped value.");

      vk::Extent2D extent = window_size;

      extent.width = std::clamp(
        extent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width);

      extent.height = std::clamp(
        extent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height);

      return extent;
    } else {
      return capabilities.currentExtent;
    }
  }

  vk::SurfaceTransformFlagBitsKHR
    chooseSwapchainPreTransform(const vk::SurfaceCapabilitiesKHR& capabilities)
  {
    return capabilities.currentTransform;
  }

  vk::CompositeAlphaFlagBitsKHR chooseSwapchainCompositeAlpha(
    const vk::SurfaceCapabilitiesKHR& capabilities)
  {
    using namespace yave;

    if (
      capabilities.supportedCompositeAlpha &
      vk::CompositeAlphaFlagBitsKHR::eOpaque)
      return vk::CompositeAlphaFlagBitsKHR::eOpaque;

    Warning(
      g_vulkan_logger,
      "vk::CompositeAlphaFlagBitsKHR::eOpaque is not supported. Use eInherit");

    return vk::CompositeAlphaFlagBitsKHR::eInherit;
  }

  vk::UniqueSwapchainKHR createSwapchain(
    const vk::SurfaceKHR& surface,
    const vk::Extent2D windowExtent,
    uint32_t graphicsQueueIndex,
    uint32_t presentQueueIndex,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& logicalDevice)
  {
    using namespace yave;

    auto availFormats  = physicalDevice.getSurfaceFormatsKHR(surface);
    auto format        = chooseSurfaceFormat(availFormats);

    Info(
      g_vulkan_logger,
      "Surface format: {}/{}",
      vk::to_string(format.format),
      vk::to_string(format.colorSpace));

    auto availModes = physicalDevice.getSurfacePresentModesKHR(surface);
    auto mode       = choosePresentMode(availModes);

    Info(g_vulkan_logger, "Present mode: {}", vk::to_string(mode));

    auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    auto extent       = chooseSwapchainExtent(windowExtent, capabilities);

    Info(
      g_vulkan_logger, "Swapchain extent: {},{}", extent.width, extent.height);

    uint32_t imageCount = std::clamp(
      capabilities.minImageCount + 1,
      capabilities.minImageCount,
      capabilities.maxImageCount);

    Info(g_vulkan_logger, "Swapchain count: {}", imageCount);

    auto preTransform   = chooseSwapchainPreTransform(capabilities);

    Info(
      g_vulkan_logger,
      "Swapchain pre transform: {}",
      vk::to_string(preTransform));

    auto compositeAlpha = chooseSwapchainCompositeAlpha(capabilities);

    Info(
      g_vulkan_logger,
      "Swapchain composite alpha: {}",
      vk::to_string(compositeAlpha));

    vk::SwapchainCreateInfoKHR info;
    info.flags            = vk::SwapchainCreateFlagsKHR();
    info.surface          = surface;
    info.imageFormat      = format.format;
    info.imageColorSpace  = format.colorSpace;
    info.imageExtent      = extent;
    info.presentMode      = mode;
    info.preTransform     = preTransform;
    info.compositeAlpha   = compositeAlpha;
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

    Info(
      g_vulkan_logger,
      "Swapchain image sharing mode: {}",
      vk::to_string(info.imageSharingMode));

    auto swapchain = logicalDevice.createSwapchainKHRUnique(info);
    Info(g_vulkan_logger, "Created new swapchain");
    return swapchain;
  }

  vk::ComponentMapping chooseImageViewComponentMapping()
  {
    return vk::ComponentMapping(
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity);
  }

  vk::ImageSubresourceRange chooseImageViewSubResourceRange()
  {
    return vk::ImageSubresourceRange(
      vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
  }

  std::vector<vk::UniqueImageView> createSwapchainImageViews(
    const vk::SurfaceKHR& surface,
    const vk::SwapchainKHR& swapchain,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& device)
  {
    using namespace yave;

    auto swapchainImages = device.getSwapchainImagesKHR(swapchain);

    std::vector<vk::UniqueImageView> ret;
    ret.reserve(swapchainImages.size());

    auto componentMapping = chooseImageViewComponentMapping();

    Info(
      g_vulkan_logger,
      "Image view component mapping: {},{},{},{}",
      vk::to_string(componentMapping.r),
      vk::to_string(componentMapping.g),
      vk::to_string(componentMapping.b),
      vk::to_string(componentMapping.a));

    auto subResourceRange = chooseImageViewSubResourceRange();

    Info(
      g_vulkan_logger,
      "Image view sub resource range: {}",
      vk::to_string(subResourceRange.aspectMask));

    // TODO: find better way to set image format
    auto availFormats = physicalDevice.getSurfaceFormatsKHR(surface);
    auto format       = chooseSurfaceFormat(availFormats);

    Info(
      g_vulkan_logger,
      "Swapchain image view format: {}",
      vk::to_string(format.format));

    for (auto&& image : swapchainImages) {
      vk::ImageViewCreateInfo info;
      info.flags    = vk::ImageViewCreateFlags();
      info.image    = image;
      info.format   = format.format;
      info.viewType = vk::ImageViewType::e2D;

      auto view = device.createImageViewUnique(info);

      if (!view)
        throw std::runtime_error("Failed to create swapchain image view");

      ret.push_back(std::move(view));
    }

    return ret;
  }

  std::vector<vk::UniqueFramebuffer> createFrameBuffers(
    const std::vector<vk::ImageView>& image_views,
    const vk::RenderPass& render_pass,
    const vk::Extent2D swapchainExtent,
    const vk::Device& device)
  {
    std::vector<vk::UniqueFramebuffer> ret;
    ret.reserve(image_views.size());

    for (auto&& view : image_views) {

      auto attachments = std::vector {view};

      vk::FramebufferCreateInfo info;
      info.flags           = vk::FramebufferCreateFlags();
      info.renderPass      = render_pass;
      info.attachmentCount = attachments.size();
      info.pAttachments    = attachments.data();
      info.width           = swapchainExtent.width;
      info.height          = swapchainExtent.height;
      info.layers          = 1;

      auto buff = device.createFramebufferUnique(info);
      if (!buff)
        throw std::runtime_error("Failed to create frame buffer");

      ret.push_back(std::move(buff));
    }

    return ret;
  }

  std::vector<vk::AttachmentDescription> getRenderPassColorAttachments(
    const vk::SurfaceKHR& surface,
    const vk::PhysicalDevice& physicalDevice)
  {
    auto availFormats = physicalDevice.getSurfaceFormatsKHR(surface);
    auto format       = chooseSurfaceFormat(availFormats);

    vk::AttachmentDescription colorAttachment;

    // swap chain image format
    colorAttachment.format = format.format;
    // sample count
    colorAttachment.samples = vk::SampleCountFlagBits::e1;

    // behaviour before rendering
    // eClear: clear
    // eLoad: preserve
    // eDontCare: undefined
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;

    // behaviour before rendering
    // eStore: store rendered content to memory
    // eDontCare: undefined
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;

    // dont care about stencil
    colorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    // don't care initial layout (we clear it anyway)
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    // follow swap chain
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    return {colorAttachment};
  }

  std::vector<vk::AttachmentReference> getRenderPassAttachmentReferences()
  {
    vk::AttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;
    return {colorAttachmentRef};
  }

  std::pair<
    std::vector<vk::SubpassDescription>,
    std::tuple<std::vector<vk::AttachmentReference>>>
    getSubpasses(
      [[maybe_unused]] const vk::SurfaceKHR& surface,
      [[maybe_unused]] const vk::PhysicalDevice& physicalDevice)
  {
    std::vector<vk::AttachmentReference> colorAttachmentRef =
      getRenderPassAttachmentReferences();

    std::vector<vk::SubpassDescription> subpass(1);
    subpass[0].pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
    subpass[0].colorAttachmentCount = colorAttachmentRef.size();
    subpass[0].pColorAttachments    = colorAttachmentRef.data();

    // forward resource to caller with std::make_*
    return std::make_pair(
      subpass, std::make_tuple(std::move(colorAttachmentRef)));
  }

  vk::SubpassDependency getSubpassDependency()
  {
    vk::SubpassDependency dep;
    dep.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dep.dstSubpass    = 0;
    dep.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dep.srcAccessMask = {};
    dep.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                        vk::AccessFlagBits::eColorAttachmentWrite;
    return dep;
  }

  vk::UniqueRenderPass createRenderPass(
    const vk::SurfaceKHR& surface,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& device)
  {
    using namespace yave;

    vk::RenderPassCreateInfo info;

    auto attachments = getRenderPassColorAttachments(surface, physicalDevice);
    info.attachmentCount = attachments.size();
    info.pAttachments    = attachments.data();

    auto [subpasses, subpassesResource] = getSubpasses(surface, physicalDevice);
    info.subpassCount                   = subpasses.size();
    info.pSubpasses                     = subpasses.data();

    auto dependency      = getSubpassDependency();
    info.dependencyCount = 1;
    info.pDependencies   = &dependency;

    auto renderPass = device.createRenderPassUnique(info);

    if (!renderPass)
      throw std::runtime_error("Failed create render pass");

    return renderPass;
  }

  vk::UniquePipelineLayout createPipelineLayout(const vk::Device& device)
  {
    vk::PipelineLayoutCreateInfo info {};
    info.flags                  = {};
    info.setLayoutCount         = 0;
    info.pSetLayouts            = nullptr;
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges    = nullptr;

    auto layout = device.createPipelineLayoutUnique(info);

    if (!layout)
      throw std::runtime_error("Failed to create pipeline layout");

    return layout;
  }

  vk::UniquePipelineCache createPipelineCache(const vk::Device& device)
  {
    vk::PipelineCacheCreateInfo info {};
    // reserved
    info.flags = {};
    // initial data
    info.initialDataSize = 0;
    info.pInitialData    = nullptr;

    auto cache = device.createPipelineCacheUnique(info);

    if (!cache)
      throw std::runtime_error("Failed to create pipeline cache");

    return cache;
  }
} // namespace

namespace yave {

  vulkan_context::vulkan_context(
    [[maybe_unused]] glfw_context& glfw_ctx,
    bool enableValidationLayer)
  {
    init_vulkan_logger();

    if (!glfwVulkanSupported())
      throw std::runtime_error("GLFW could not find Vulkan");

    /* instance */

    m_instance = createInstance(enableValidationLayer);
    m_debugCallback =
      createDebugReportCallback(enableValidationLayer, m_instance.get());

    /* physical device */

    m_physicalDevice           = acquirePhysicalDevice(m_instance.get());
    m_physicalDeviceProperties = m_physicalDevice.getProperties();
    m_physicalDeviceQueueFamilyProperties =
      m_physicalDevice.getQueueFamilyProperties();

    /* device queue settings */

    m_graphicsQueueIndex = getGraphicsQueueIndex(m_physicalDevice);

    Info(g_vulkan_logger, "Graphs queue idex: {}", m_graphicsQueueIndex);

    m_presentQueueIndex = getPresentQueueIndex(
      m_graphicsQueueIndex, m_instance.get(), m_physicalDevice);

    Info(g_vulkan_logger, "Presentation queue index: {}", m_presentQueueIndex);

    m_queueFamilyIndicies =
      getUniqueQueueFamilyIndicies({m_graphicsQueueIndex, m_presentQueueIndex});

    /* logical device */

    m_device = createDevice(m_queueFamilyIndicies, m_physicalDevice);

    Info(g_vulkan_logger, "Initialized Vulkan context");
  }

  vulkan_context::~vulkan_context() noexcept
  {
    Info(g_vulkan_logger, "Destroying Vulkan context");
  }

  vk::Instance vulkan_context::instance() const
  {
    return m_instance.get();
  }

  vk::PhysicalDevice vulkan_context::physical_device() const
  {
    return m_physicalDevice;
  }

  vk::Device vulkan_context::device() const
  {
    return m_device.get();
  }

  vk::UniqueSurfaceKHR vulkan_context::create_window_surface(
    const std::unique_ptr<GLFWwindow, glfw_window_deleter>& window) const
  {
    VkSurfaceKHR surface;

    auto err = glfwCreateWindowSurface(
      m_instance.get(), window.get(), nullptr, &surface);

    if (err != VK_SUCCESS) {
      throw std::runtime_error("Failed to create window surface");
    }

    Info(g_vulkan_logger, "Created new window surface");

    vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderStatic> deleter(
      m_instance.get());
    return vk::UniqueSurfaceKHR(surface, deleter);
  }

  vk::UniqueSwapchainKHR vulkan_context::create_surface_swapchain(
    const vk::UniqueSurfaceKHR& surface,
    const std::unique_ptr<GLFWwindow, glfw_window_deleter>& window) const
  {
    // TODO: cache swapchain extent
    int width, height;
    glfwGetFramebufferSize(window.get(), &width, &height);

    assert(width > 0 && height > 0);

    return createSwapchain(
      surface.get(),
      {static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
      m_graphicsQueueIndex,
      m_presentQueueIndex,
      m_physicalDevice,
      m_device.get());
  }

  std::vector<vk::UniqueImageView> vulkan_context::create_swapchain_image_views(
    const vk::UniqueSurfaceKHR& surface,
    const vk::UniqueSwapchainKHR& swapchain) const
  {
    auto image_views = createSwapchainImageViews(
      surface.get(), swapchain.get(), m_physicalDevice, m_device.get());
    Info(g_vulkan_logger, "Created swapchain image views");
    return image_views;
  }

  vk::UniqueRenderPass vulkan_context::create_render_pass(
    const vk::UniqueSurfaceKHR& surface,
    const vk::UniqueSwapchainKHR& swapchain) const
  {
    (void)swapchain;
    auto pass =
      createRenderPass(surface.get(), m_physicalDevice, m_device.get());
    Info(g_vulkan_logger, "Created render pass");
    return pass;
  }

  std::vector<vk::UniqueFramebuffer> vulkan_context::create_frame_buffers(
    const vk::UniqueSurfaceKHR& surface,
    const std::unique_ptr<GLFWwindow, glfw_window_deleter>& window,
    const std::vector<vk::UniqueImageView>& swapchain_views,
    const vk::UniqueRenderPass& render_pass) const
  {
    std::vector<vk::ImageView> tmp_views;
    for (auto&& view : swapchain_views) {
      tmp_views.push_back(view.get());
    }

    // TODO: cache swapchain extent
    int width, height;
    glfwGetFramebufferSize(window.get(), &width, &height);

    assert(width > 0 && height > 0);

    auto buffs = createFrameBuffers(
      tmp_views,
      render_pass.get(),
      {static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
      m_device.get());

    Info(g_vulkan_logger, "Created frame buffers");
    return buffs;
  }

  vk::UniquePipelineLayout vulkan_context::create_pipeline_layout() const
  {
    auto layout = createPipelineLayout(m_device.get());
    Info(g_vulkan_logger, "Created pipeline layout");
    return layout;
  }

  vk::UniquePipelineCache vulkan_context::create_pipeline_cache() const
  {
    auto cache = createPipelineCache(m_device.get());
    Info(g_vulkan_logger, "Created pipeline cache");
    return cache;
  }

  std::vector<vk::SurfaceFormatKHR>
    vulkan_context::get_surface_formats(const vk::SurfaceKHR& surface) const
  {
    using namespace yave;
    auto formats = m_physicalDevice.getSurfaceFormatsKHR(surface);
    Info(g_vulkan_logger, "Query surface formats:");
    for (auto&& f : formats) {
      Info(
        g_vulkan_logger,
        "  {} {}",
        vk::to_string(f.format),
        vk::to_string(f.colorSpace));
    }
    return formats;
  }

} // namespace yave