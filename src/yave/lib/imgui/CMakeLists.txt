# yave::lib::imgui

add_library(yave-lib-imgui 
  imgui_context.cpp
  invisible_button_ex.cpp)

add_library(yave::lib::imgui ALIAS yave-lib-imgui)

target_link_libraries(yave-lib-imgui PRIVATE yave::config)
target_link_libraries(yave-lib-imgui PRIVATE yave::support::log)

target_link_libraries(yave-lib-imgui PUBLIC yave::lib::filesystem)
target_link_libraries(yave-lib-imgui PUBLIC yave::lib::glfw)
target_link_libraries(yave-lib-imgui PUBLIC yave::lib::vulkan)
target_link_libraries(yave-lib-imgui PUBLIC imgui-glfw-vulkan)

# Add Roboto-Medium.ttf
target_compile_definitions(yave-lib-imgui PRIVATE "-DYAVE_IMGUI_FONT_ROBOTO=${YAVE_EXTERNAL_DIR}/imgui/misc/fonts/Roboto-Medium.ttf")

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

# Add command to copy shaders into lib directory
add_custom_command(
  TARGET yave-lib-imgui POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy frag.spv $<TARGET_FILE_DIR:yave-lib-imgui>/frag.spv
  COMMAND ${CMAKE_COMMAND} -E copy vert.spv $<TARGET_FILE_DIR:yave-lib-imgui>/vert.spv
)

# And custom targets for commands
add_custom_target(
  yave-imgui-vert
  DEPENDS vert.spv
)
add_custom_target(
  yave-imgui-frag
  DEPENDS frag.spv
)

# And compiler definitions for shader paths...
add_library(yave-imgui-shaders INTERFACE)
add_dependencies(yave-imgui-shaders yave-imgui-vert)
add_dependencies(yave-imgui-shaders yave-imgui-frag)
target_compile_definitions(yave-imgui-shaders INTERFACE "-DYAVE_IMGUI_VERT_SHADER=$<TARGET_FILE_DIR:yave-lib-imgui>/vert.spv")
target_compile_definitions(yave-imgui-shaders INTERFACE "-DYAVE_IMGUI_FRAG_SHADER=$<TARGET_FILE_DIR:yave-lib-imgui>/frag.spv")

# And dependencies for target... CMake is easy isn't it?
target_link_libraries(yave-lib-imgui PRIVATE yave-imgui-shaders)