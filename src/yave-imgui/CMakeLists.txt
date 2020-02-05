
add_subdirectory(editor)

# yave-imgui

add_executable(yave-imgui main.cpp)
target_include_directories(yave-imgui PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui PRIVATE yave::lib::imgui)
target_link_libraries(yave-imgui PRIVATE yave::editor::imgui)
target_link_libraries(yave-imgui PRIVATE yave::support)
target_link_libraries(yave-imgui PRIVATE yave::node)
target_link_libraries(yave-imgui PRIVATE yave::module::std)