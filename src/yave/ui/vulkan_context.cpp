//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/vulkan_context.hpp>
#include <yave/support/log.hpp>

#include <ranges>

YAVE_DECL_LOCAL_LOGGER(ui::vulkan_context)

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

namespace yave::ui {

  namespace rn = std::ranges;
  namespace rv = std::ranges::views;

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
          log_info("{}", pMessage);
          break;
        case VK_DEBUG_REPORT_WARNING_BIT_EXT:
          log_warning("{}", pMessage);
          break;
        case VK_DEBUG_REPORT_ERROR_BIT_EXT:
          log_error("{}", pMessage);
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

    /// Check vk::PhysicalDeviceFeatures compatibility
    bool checkPhysicalDeviceFeatureSupport(
      const vk::PhysicalDeviceFeatures& required,
      const vk::PhysicalDeviceFeatures& supported)
    {
#define YAVE_VULKAN_DEVICE_FEATURE_TEST_H(FEATURE) \
  if (required.FEATURE && !supported.FEATURE)      \
  return false

      // clang-format off
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
      // clang-format on
      return true;
    }

    auto getInstanceExtensions()
    {
      auto requiredExtensions = std::array {
        VK_KHR_SURFACE_EXTENSION_NAME,     // for surface
        PlatformSurfaceExtensionName,      // for surface
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME // for validation
      };
      auto supportedExtensions = vk::enumerateInstanceExtensionProperties();

      for (auto&& e : requiredExtensions) {

        auto iter = rn::find_if(supportedExtensions, [&](auto&& x) {
          return std::string_view(x.extensionName.data()) == e;
        });

        if (iter == supportedExtensions.end())
          throw std::runtime_error("Unsupported Vulkan instance extension");
      }
      return requiredExtensions;
    }

    auto getInstanceLayers()
    {
      auto requiredLayers  = std::array {"VK_LAYER_KHRONOS_validation"};
      auto supportedLayers = vk::enumerateInstanceLayerProperties();

      for (auto&& l : requiredLayers) {

        auto iter = rn::find_if(supportedLayers, [&](auto&& x) {
          return std::string_view(x.layerName.data()) == l;
        });

        if (iter == supportedLayers.end())
          throw std::runtime_error("Unsupoprted Vulkan instance layer");
      }
      return requiredLayers;
    }

    void loadDebugReportExtensions(const vk::UniqueInstance& inst)
    {
      assert(inst.get());

      pfn_vkCreateDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
          inst->getProcAddr("vkCreateDebugReportCallbackEXT"));
      pfn_vkDestroyDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
          inst->getProcAddr("vkDestroyDebugReportCallbackEXT"));

      if (!pfn_vkCreateDebugReportCallbackEXT)
        throw std::runtime_error(
          "Failed to load function vkCreateDebugReportCallbackExt");
      if (!pfn_vkDestroyDebugReportCallbackEXT)
        throw std::runtime_error(
          "Failed to load function vkDestroyDebugReportCallbackExt");
    }

    auto createInstance() -> vk::UniqueInstance
    {
      auto appInfo = vk::ApplicationInfo(
        "yave",
        VK_MAKE_VERSION(0, 0, 0),
        "No Engine",
        VK_MAKE_VERSION(0, 0, 0),
        VK_API_VERSION_1_1);

      auto layers     = getInstanceLayers();
      auto extensions = getInstanceExtensions();

      auto info = vk::InstanceCreateInfo()
                    .setPApplicationInfo(&appInfo)
                    .setPEnabledLayerNames(layers)
                    .setPEnabledExtensionNames(extensions);

      auto instance = vk::createInstanceUnique(info);
      loadDebugReportExtensions(instance);
      return instance;
    }

    auto createDebugReportCallback(const vk::UniqueInstance& instance)
      -> vk::UniqueDebugReportCallbackEXT
    {
      assert(instance.get());

      auto info = vk::DebugReportCallbackCreateInfoEXT()
                    .setFlags(
                      vk::DebugReportFlagBitsEXT::eError
                      | vk::DebugReportFlagBitsEXT::eWarning)
                    .setPfnCallback(validationCallback);

      return instance->createDebugReportCallbackEXTUnique(info);
    }

    bool physicalDeviceMeetsRequirements(
      const vk::PhysicalDevice& physicalDevice)
    {
      // types
      {
        auto properties    = physicalDevice.getProperties();
        auto requiredTypes = std::array {
          vk::PhysicalDeviceType::eCpu,
          vk::PhysicalDeviceType::eDiscreteGpu,
          vk::PhysicalDeviceType::eIntegratedGpu,
          vk::PhysicalDeviceType::eOther,
          vk::PhysicalDeviceType::eVirtualGpu};

        auto it = rn::find(requiredTypes, properties.deviceType);

        if (it == requiredTypes.end())
          return false;
      }

      // features
      {
        auto features = physicalDevice.getFeatures();

        auto requiredFeatures = vk::PhysicalDeviceFeatures()
                                  .setGeometryShader(VK_TRUE)
                                  .setSampleRateShading(VK_TRUE);

        if (!checkPhysicalDeviceFeatureSupport(requiredFeatures, features))
          return false;
      }

      // queue family properties
      {
        auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

        auto requiredQueueFlags = std::array {
          vk::QueueFlagBits::eGraphics,
          vk::QueueFlagBits::eTransfer,
          vk::QueueFlagBits::eCompute};

        for (auto&& flag : requiredQueueFlags) {

          auto it = std::find_if(
            queueFamilyProperties.begin(),
            queueFamilyProperties.end(),
            [&](auto&& p) { return p.queueFlags & flag; });

          if (it == queueFamilyProperties.end())
            return false;
        }
      }
      return true;
    }

    auto selectDefaultPhysicalDevice(const vk::UniqueInstance& instance)
    {
      auto physicalDevices = instance->enumeratePhysicalDevices();

      if (physicalDevices.empty())
        throw std::runtime_error("Not Vulkan physical device available");

      for (auto&& physicalDevice : physicalDevices) {
        if (physicalDeviceMeetsRequirements(physicalDevice)) {
          return physicalDevice;
        }
      }
      throw std::runtime_error("Not physical device meets requirements");
    }

  } // namespace

  vulkan_context::vulkan_context()
  {
    m_instance            = createInstance();
    m_debugReportCallback = createDebugReportCallback(m_instance);
    m_physicalDevice      = selectDefaultPhysicalDevice(m_instance);
  }

} // namespace yave::ui