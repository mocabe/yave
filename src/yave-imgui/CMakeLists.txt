# yave-imgui

set(SOURCES 
  application.cpp
  root_window.cpp
  node_window.cpp
  info_window.cpp
  render_view_window.cpp
  node_window_drawables.cpp
  basic_node_drawer.cpp
  basic_socket_drawer.cpp
  basic_connection_drawer.cpp
  data_type_socket.cpp
  data_commands.cpp
  layout.cpp
  main.cpp
) 

yave_glob_include_files(_include_files)

source_group("src" FILES ${SOURCES})
source_group("include" FILES ${_include_files})

set(SOURCES ${SOURCES} ${_include_files})
add_executable(yave-imgui ${SOURCES})

set_target_properties(yave-imgui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(yave-imgui PROPERTIES FOLDER "yave-imgui")

target_link_libraries(yave-imgui PRIVATE yave::core)
target_link_libraries(yave-imgui PRIVATE yave::lib::imgui)
target_link_libraries(yave-imgui PRIVATE yave::support)
target_link_libraries(yave-imgui PRIVATE yave::node)
target_link_libraries(yave-imgui PRIVATE yave::data::node)
target_link_libraries(yave-imgui PRIVATE yave::module::std)
target_link_libraries(yave-imgui PRIVATE yave::wm)