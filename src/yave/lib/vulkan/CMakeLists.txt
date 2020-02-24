# yave::lib::vulkan

add_library(yave-lib-vulkan 
  vulkan_context.cpp 
  window_context.cpp
  vulkan_util.cpp
)

add_library(yave::lib::vulkan ALIAS yave-lib-vulkan)

target_link_libraries(yave-lib-vulkan PRIVATE yave::config)
target_link_libraries(yave-lib-vulkan PRIVATE yave::support::log)
target_link_libraries(yave-lib-vulkan PUBLIC yave::lib::glfw)
target_link_libraries(yave-lib-vulkan PUBLIC vulkan)
target_link_libraries(yave-lib-vulkan PUBLIC selene)