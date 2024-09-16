# Vulkan
# ${Vulkan_INCLUDE_DIRS}
# ${Vulkan_LIBRARIES}
find_package(Vulkan REQUIRED)
add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vulkan INTERFACE ${Vulkan_LIBRARIES})

# Vulkan Memory Allocator
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)
target_link_libraries(vulkan INTERFACE unofficial::vulkan-memory-allocator::vulkan-memory-allocator)