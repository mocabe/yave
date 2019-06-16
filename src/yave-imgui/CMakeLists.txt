# yave::imgui_glfw_vulkan

add_library(yave-imgui_glfw_vulkan imgui_glfw_vulkan.cpp)
add_library(yave::imgui_glfw_vulkan ALIAS yave-imgui_glfw_vulkan)
target_include_directories(yave-imgui_glfw_vulkan PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui_glfw_vulkan PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui_glfw_vulkan PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui_glfw_vulkan PUBLIC yave::support::log)
target_link_libraries(yave-imgui_glfw_vulkan PUBLIC yave::support::glfw_vulkan)
target_link_libraries(yave-imgui_glfw_vulkan PUBLIC imgui-glfw-vulkan)
target_link_libraries(yave-imgui_glfw_vulkan PRIVATE Boost::boost)
target_link_libraries(yave-imgui_glfw_vulkan PRIVATE Boost::filesystem)

# Add commands to compile shaders
add_custom_command(
  OUTPUT vert.spv
  COMMAND glslangValidator -V ${CMAKE_CURRENT_LIST_DIR}/shader.vert -o vert.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_LIST_DIR}/shader.vert
)
add_custom_command(
  OUTPUT frag.spv
  COMMAND glslangValidator -V ${CMAKE_CURRENT_LIST_DIR}/shader.frag -o frag.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_LIST_DIR}/shader.frag
)

# And add custom targets for commands because it's CMake
add_custom_target(
  yave-imgui_glfw_vulkan-vert
  DEPENDS vert.spv
)
add_custom_target(
  yave-imgui_glfw_vulkan-frag
  DEPENDS frag.spv
)

# And dependencies for target... CMake is easy isn't it?
add_dependencies(yave-imgui_glfw_vulkan yave-imgui_glfw_vulkan-vert)
add_dependencies(yave-imgui_glfw_vulkan yave-imgui_glfw_vulkan-frag)

# yave-imgui

add_executable(yave-imgui yave-imgui.cpp)
target_include_directories(yave-imgui PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui PRIVATE yave::imgui_glfw_vulkan)
