# yave::lib::vulkan

add_library(yave-lib-vulkan 
  vulkan_context.cpp 
  shader.cpp
  texture.cpp
  offscreen.cpp
  render_buffer.cpp
  window_context.cpp
  vulkan_util.cpp
  offscreen_render_pass.cpp
  offscreen_renderer_2D.cpp
  draw2d.cpp
)

add_library(yave::lib::vulkan ALIAS yave-lib-vulkan)

target_link_libraries(yave-lib-vulkan PRIVATE yave::config)
target_link_libraries(yave-lib-vulkan PRIVATE yave::support::log)
target_link_libraries(yave-lib-vulkan PUBLIC yave::lib::image)
target_link_libraries(yave-lib-vulkan PUBLIC yave::lib::glfw)
target_link_libraries(yave-lib-vulkan PUBLIC vulkan)
target_link_libraries(yave-lib-vulkan PUBLIC selene)
target_link_libraries(yave-lib-vulkan PUBLIC glm)
target_link_libraries(yave-lib-vulkan PUBLIC glslang)
target_link_libraries(yave-lib-vulkan PUBLIC SPIRV)
target_link_libraries(yave-lib-vulkan PUBLIC glslang-default-resource-limits)