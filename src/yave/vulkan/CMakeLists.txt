# yave::vulkan

add_library(yave-vulkan vulkan_context.cpp vulkan_util.cpp)
add_library(yave::vulkan ALIAS yave-vulkan)

target_include_directories(yave-vulkan PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-vulkan PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-vulkan PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-vulkan PRIVATE yave-support-log)

target_link_libraries(yave-vulkan PUBLIC yave::glfw)
target_link_libraries(yave-vulkan PUBLIC vulkan)
target_link_libraries(yave-vulkan PUBLIC selene)