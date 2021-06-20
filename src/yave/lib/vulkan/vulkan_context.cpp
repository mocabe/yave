//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#define GLFW_INCLUDE_VULKAN

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/core/log.hpp>

YAVE_DECL_LOCAL_LOGGER(vulkan)

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

    switch (flags) {
      case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
        log_info( "{}", pMessage);
        break;
      case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        log_warning( "{}", pMessage);
        break;
      case VK_DEBUG_REPORT_ERROR_BIT_EXT:
        log_error( "{}", pMessage);
        break;
    }
    return VK_FALSE;
  }

  // instance surface extension
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_IOS_MVK)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_MVK_IOS_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_MIR_KHR)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_MIR_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_VI_NN)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_NN_VI_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_XCB_KHR)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_XLIB_XRANDR_EXT)
  constexpr const char* PlatformSurfaceExtensionName =
    VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME;
#else
  constexpr const char* PlatformSurfaceExtensionName =
    VK_KHR_SURFACE_EXTENSION_NAME;
#endif

  // validation layer name
  constexpr const char* ValidationLayerName = "VK_LAYER_KHRONOS_validation";

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

    // default extensions
    constexpr std::array extensions = {
      VK_KHR_SURFACE_EXTENSION_NAME, // for surface
      PlatformSurfaceExtensionName,  // for surface
    };

    // for validation layer
    constexpr const char* debugReportExtension = DebugReportExtensionName;

    log_info( "Debug spec version: {}", DebugReportSpecVersion);

    // collect enabling extensions

    std::vector<std::string> ret;

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

    log_info( "Following instance extensions will be enabled:");
    for (auto&& e : extensions)
      log_info( " {}", e);

    log_info( "Following instance layers will be enabled:");
    for (auto&& l : layers)
      log_info( "  {}", l);

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

} // namespace

namespace yave::vulkan {

  // -----------------------------------------
  // vulkan_context

  class vulkan_context::impl
  {
  public:
    /// instance
    vk::UniqueInstance instance;
    /// validation callback
    vk::UniqueDebugReportCallbackEXT debugCallback;
    /// physical device
    vk::PhysicalDevice physicalDevice;
    /// property of physical device
    vk::PhysicalDeviceProperties physicalDeviceProperties;
    /// queue family property of physical device
    std::vector<vk::QueueFamilyProperties> physicalDeviceQueueFamilyProperties;

  public:
    impl(create_info info)
    {
      /* instance */

      bool enableValidationLayer = info.enable_validation;

      instance = createInstance(enableValidationLayer);
      debugCallback =
        createDebugReportCallback(enableValidationLayer, instance.get());

      /* physical device */

      physicalDevice           = acquirePhysicalDevice(instance.get());
      physicalDeviceProperties = physicalDevice.getProperties();
      physicalDeviceQueueFamilyProperties =
        physicalDevice.getQueueFamilyProperties();

      log_info( "Initialized Vulkan context");
    }

    bool check_physical_device_extension_support(
      const std::vector<std::string>& extensions)
    {
      auto supportedExtensions =
        physicalDevice.enumerateDeviceExtensionProperties();

      for (auto&& e : extensions) {
        auto it = std::find_if(
          supportedExtensions.begin(), supportedExtensions.end(), [&](auto& x) {
            return (const char*)x.extensionName == e;
          });
        if (it == supportedExtensions.end())
          return false;
      }
      return true;
    }

    bool check_physical_device_layer_support(
      const std::vector<std::string>& layers)
    {
      auto supportedLayers = physicalDevice.enumerateDeviceLayerProperties();

      for (auto&& l : layers) {
        auto it = std::find_if(
          supportedLayers.begin(), supportedLayers.end(), [&](auto& x) {
            return (const char*)x.layerName == l;
          });
        if (it == supportedLayers.end())
          return false;
      }
      return true;
    }

    auto create_device(
      std::vector<uint32_t> queueIndicies,
      const std::vector<std::string>& extensions,
      const std::vector<std::string>& layers)
    {
      // Vulkan API requires queue family indicies to be unqiue.
      {
        std::sort(queueIndicies.begin(), queueIndicies.end());
        auto end = std::unique(queueIndicies.begin(), queueIndicies.end());
        queueIndicies.erase(end, queueIndicies.end());
      }

      float queuePriority = 0.f;

      std::vector<vk::DeviceQueueCreateInfo> createInfoList;
      for (auto index : queueIndicies) {
        vk::DeviceQueueCreateInfo info;
        info.flags            = vk::DeviceQueueCreateFlags();
        info.queueFamilyIndex = index;
        info.queueCount       = 1;
        info.pQueuePriorities = &queuePriority;
        createInfoList.push_back(std::move(info));
      }

      if (!check_physical_device_extension_support(extensions))
        throw std::runtime_error("Device extension not supported");
      if (!check_physical_device_layer_support(layers))
        throw std::runtime_error("Device layer not supported");

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
  };

  vulkan_context::vulkan_context(create_info info)
    : m_pimpl {std::make_unique<impl>(info)}
  {
  }

  vulkan_context::~vulkan_context() noexcept
  {
    log_info( "Destroying Vulkan context");
  }

  auto vulkan_context::instance() const -> vk::Instance
  {
    return m_pimpl->instance.get();
  }

  auto vulkan_context::physical_device() const -> vk::PhysicalDevice
  {
    return m_pimpl->physicalDevice;
  }

  auto vulkan_context::physical_device_properties() const
    -> const vk::PhysicalDeviceProperties&
  {
    return m_pimpl->physicalDeviceProperties;
  }

  auto vulkan_context::physical_device_queue_properties() const
    -> const std::vector<vk::QueueFamilyProperties>&
  {
    return m_pimpl->physicalDeviceQueueFamilyProperties;
  }

  bool vulkan_context::check_physical_device_extension_support(
    const std::vector<std::string>& extensions) const
  {
    return m_pimpl->check_physical_device_extension_support(extensions);
  }

  bool vulkan_context::check_physical_device_layer_support(
    const std::vector<std::string>& layers) const
  {
    return m_pimpl->check_physical_device_layer_support(layers);
  }

  auto vulkan_context::create_device(
    const std::vector<uint32_t> queue_indicies,
    const std::vector<std::string>& extensions,
    const std::vector<std::string>& layers) -> vk::UniqueDevice
  {
    return m_pimpl->create_device(queue_indicies, extensions, layers);
  }
} // namespace yave::vulkan