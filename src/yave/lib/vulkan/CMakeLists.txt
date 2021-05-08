# yave::lib::vulkan

add_library(yave-lib-vulkan 
  vulkan_context.cpp 
  shader.cpp
  staging.cpp
  texture.cpp
  offscreen.cpp
  render_buffer.cpp
  window_context.cpp
  vulkan_util.cpp
  offscreen_context.cpp
  offscreen_render_pass.cpp
  offscreen_compositor.cpp
  vk_mem_alloc.cpp
)

add_library(yave::lib::vulkan ALIAS yave-lib-vulkan)

target_link_libraries(yave-lib-vulkan PRIVATE yave::config)
target_link_libraries(yave-lib-vulkan PRIVATE yave::support::log)
target_link_libraries(yave-lib-vulkan PUBLIC yave::lib::image)
target_link_libraries(yave-lib-vulkan PUBLIC yave::lib::glfw)
target_link_libraries(yave-lib-vulkan PUBLIC vulkan)
target_link_libraries(yave-lib-vulkan PUBLIC glm::glm)
target_link_libraries(yave-lib-vulkan PUBLIC glslang)
target_link_libraries(yave-lib-vulkan PUBLIC SPIRV)
target_link_libraries(yave-lib-vulkan PUBLIC glslang-default-resource-limits)

if(MSVC OR MINGW)
  target_compile_definitions(yave-lib-vulkan PUBLIC VK_USE_PLATFORM_WIN32_KHR)
else()
  target_compile_definitions(yave-lib-vulkan PUBLIC VK_USE_PLATFORM_XCB_KHR)
endif()