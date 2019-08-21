# yave::lib::vulkan

add_library(yave-lib-vulkan vulkan_context.cpp vulkan_util.cpp)
add_library(yave::lib::vulkan ALIAS yave-lib-vulkan)

target_include_directories(yave-lib-vulkan PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-lib-vulkan PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-lib-vulkan PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-lib-vulkan PRIVATE yave-support-log)

target_link_libraries(yave-lib-vulkan PUBLIC yave::lib::glfw)
target_link_libraries(yave-lib-vulkan PUBLIC vulkan)
target_link_libraries(yave-lib-vulkan PUBLIC selene)