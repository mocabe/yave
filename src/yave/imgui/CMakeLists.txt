# yave::imgui

add_library(yave-imgui imgui_context.cpp)
add_library(yave::imgui ALIAS yave-imgui)

target_include_directories(yave-imgui PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui PRIVATE yave::support::log)
target_link_libraries(yave-imgui PRIVATE Boost::boost)
target_link_libraries(yave-imgui PRIVATE Boost::filesystem)
target_link_libraries(yave-imgui PRIVATE Boost::disable_autolinking)

target_link_libraries(yave-imgui PUBLIC yave::glfw)
target_link_libraries(yave-imgui PUBLIC yave::vulkan)
target_link_libraries(yave-imgui PUBLIC imgui-glfw-vulkan)

# Add commands to compile shaders
add_custom_command(
  OUTPUT vert.spv
  COMMAND glslangValidator -V ${CMAKE_CURRENT_LIST_DIR}/shader.vert -o vert.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_LIST_DIR}/shader.vert
  WORKING_DIRECTORY $<TARGET_FILE_DIR:yave-imgui>
)
add_custom_command(
  OUTPUT frag.spv
  COMMAND glslangValidator -V ${CMAKE_CURRENT_LIST_DIR}/shader.frag -o frag.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_LIST_DIR}/shader.frag
  WORKING_DIRECTORY $<TARGET_FILE_DIR:yave-imgui>
)

# And add custom targets for commands because it's CMake
add_custom_target(
  yave-imgui-vert
  DEPENDS vert.spv
)
add_custom_target(
  yave-imgui-frag
  DEPENDS frag.spv
)

# And dependencies for target... CMake is easy isn't it?
add_dependencies(yave-imgui yave-imgui-vert)
add_dependencies(yave-imgui yave-imgui-frag)