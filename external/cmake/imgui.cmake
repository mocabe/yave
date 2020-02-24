# imgui
init_submodule(imgui)

add_library(imgui 
  ${YAVE_EXTERNAL_DIR}/imgui/imgui.cpp
  ${YAVE_EXTERNAL_DIR}/imgui/imgui_widgets.cpp
  ${YAVE_EXTERNAL_DIR}/imgui/imgui_draw.cpp
  ${YAVE_EXTERNAL_DIR}/imgui/imgui_demo.cpp
  ${YAVE_EXTERNAL_DIR}/imgui/misc/freetype/imgui_freetype.cpp
  ${YAVE_EXTERNAL_DIR}/imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC 
  ${YAVE_EXTERNAL_DIR}/imgui
  ${YAVE_EXTERNAL_DIR}/imgui/misc/freetype
  ${YAVE_EXTERNAL_DIR}/imgui/misc/cpp
)

target_link_libraries(imgui PRIVATE Freetype::Freetype)

add_library(imgui-glfw-vulkan 
  ${YAVE_EXTERNAL_DIR}/imgui/examples/imgui_impl_vulkan.cpp
  ${YAVE_EXTERNAL_DIR}/imgui/examples/imgui_impl_glfw.cpp
)
target_include_directories(imgui-glfw-vulkan PUBLIC 
  ${YAVE_EXTERNAL_DIR}/imgui/examples
  ${Vulkan_INCLUDE_DIRS}
)
target_link_libraries(imgui-glfw-vulkan PUBLIC 
  imgui
  glfw
  ${Vulkan_LIBRARIES}
)
target_compile_definitions(imgui-glfw-vulkan PUBLIC IMGUI_VULKAN_DEBUG_REPORT)

add_executable(imgui_demo ${YAVE_EXTERNAL_DIR}/imgui/examples/example_glfw_vulkan/main.cpp)
target_link_libraries(imgui_demo imgui-glfw-vulkan)