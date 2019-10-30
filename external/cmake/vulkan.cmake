# Vulkan
# ${Vulkan_INCLUDE_DIRS}
# ${Vulkan_LIBRARIES}
find_package(Vulkan REQUIRED)
add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vulkan INTERFACE ${Vulkan_LIBRARIES})