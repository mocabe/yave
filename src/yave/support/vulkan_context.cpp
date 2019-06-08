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
  std::shared_ptr<spdlog::logger> g_glfw_logger;

  // init
  void init_vulkan_logger()
  {
    [[maybe_unused]] static auto init_logger = [] {
      g_vulkan_logger = yave::add_logger("vulkan_context");
      return 1;
    }();
  }
  void init_glfw_logger()
  {
    [[maybe_unused]] static auto init_logger = [] {
      g_glfw_logger = yave::add_logger("glfw");
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

  /// GLFW erro callback
  void glfwErrorCallback(int error, const char* msg)
  {
    using namespace yave;
    (void)error;
    Error(g_glfw_logger, "{}", msg);
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

  /// ereate application info
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
    createDebugReportCallback(vk::Instance instance)
  {
    using namespace yave;
    assert(instance);

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

  std::vector<uint32_t> getQueueFamilyIndicies(
    const std::vector<vk::QueueFlagBits>& queueFlags,
    const std::vector<vk::QueueFamilyProperties>& properties)
  {
    using namespace yave;
    std::vector<uint32_t> indicies;

    for (auto&& qf : queueFlags) {
      uint32_t index = std::distance(
        properties.begin(),
        std::find_if(properties.begin(), properties.end(), [&](auto& p) {
          return p.queueFlags & qf;
        }));
      assert(index != properties.size());
      indicies.push_back(index);
    }

    Info(g_vulkan_logger, "Queue family indicies:");
    for (size_t i = 0; i < queueFlags.size(); ++i) {
      Info(
        g_vulkan_logger, "  {}: {}", vk::to_string(queueFlags[i]), indicies[i]);
    }

    return indicies;
  }

  uint32_t getPresentQueueIndex(
    const std::vector<vk::QueueFlagBits>& queueFlags,
    const std::vector<uint32_t>& queueFamilyIndicies,
    const vk::Instance& instance,
    const vk::PhysicalDevice& physicalDevice)
  {
    using namespace yave;

    // if graphics queue supports presentation, use it.
    for (uint32_t i = 0; i < queueFlags.size(); ++i) {
      if (queueFlags[i] == vk::QueueFlagBits::eGraphics) {
        if (glfwGetPhysicalDevicePresentationSupport(
              instance, physicalDevice, queueFamilyIndicies[i])) {
          Info(
            g_vulkan_logger, "Present queue index: {}", queueFamilyIndicies[i]);
          return queueFamilyIndicies[i];
        } else {
          Warning(
            g_vulkan_logger,
            "Graphics queue (index: {}) does not support "
            "presentation.",
            queueFamilyIndicies[i]);
        }
      }
    }

    // find presentation queue from all queue indicies
    for (uint32_t i = 0; i < queueFlags.size(); ++i) {
      if (glfwGetPhysicalDevicePresentationSupport(
            instance, physicalDevice, queueFamilyIndicies[i]))
        return queueFamilyIndicies[i];
    }

    Warning(
      g_vulkan_logger,
      "None of selected queue family supports presentation. Try to search from "
      "all existing queue families");

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

} // namespace

namespace yave {

  vulkan_context::vulkan_context(bool enableValidationLayer)
  {
    init_vulkan_logger();
    init_glfw_logger();

    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
      throw std::runtime_error("Failed to initialize GLFW");

    if (!glfwVulkanSupported())
      throw std::runtime_error("GLFW could not find Vulkan");

    /* instance */

    m_instance      = createInstance(enableValidationLayer);
    m_debugCallback = createDebugReportCallback(m_instance.get());

    /* physical device */

    m_physicalDevice           = acquirePhysicalDevice(m_instance.get());
    m_physicalDeviceProperties = m_physicalDevice.getProperties();
    m_physicalDeviceQueueFamilyProperties =
      m_physicalDevice.getQueueFamilyProperties();

    /* device queue settings */

    m_queueFlags          = getRequiredPhysicalDeviceQueueFlags();
    m_queueFamilyIndicies = getQueueFamilyIndicies(
      m_queueFlags, m_physicalDeviceQueueFamilyProperties);
    m_presentQueueIndex = getPresentQueueIndex(
      m_queueFlags, m_queueFamilyIndicies, m_instance.get(), m_physicalDevice);

    /* logical device */

    m_device = createDevice(m_queueFamilyIndicies, m_physicalDevice);
  }

  vulkan_context::~vulkan_context()
  {
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

} // namespace yave