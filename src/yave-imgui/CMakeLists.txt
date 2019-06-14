# yave::imgui_glfw_vulkan

add_library(yave-imgui_glfw_vulkan imgui_glfw_vulkan.cpp)
add_library(yave::imgui_glfw_vulkan ALIAS yave-imgui_glfw_vulkan)
target_include_directories(yave-imgui_glfw_vulkan PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui_glfw_vulkan PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui_glfw_vulkan PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui_glfw_vulkan PUBLIC yave::support::log)
target_link_libraries(yave-imgui_glfw_vulkan PUBLIC yave::support::glfw_vulkan)
target_link_libraries(yave-imgui_glfw_vulkan PUBLIC imgui-glfw-vulkan)


# yave-imgui

add_executable(yave-imgui yave-imgui.cpp)
target_include_directories(yave-imgui PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui PRIVATE yave::imgui_glfw_vulkan)