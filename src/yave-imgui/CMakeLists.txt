# yave-imgui

add_executable(yave-imgui 
  application.cpp
  root_window.cpp
  node_window.cpp
  render_view_window.cpp
  node_window_drawables.cpp
  basic_node_drawer.cpp
  basic_socket_drawer.cpp
  basic_connection_drawer.cpp
  data_type_socket.cpp
  layout.cpp
  main.cpp
)

target_include_directories(yave-imgui PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui PRIVATE ${YAVE_LINK_FLAGS})

target_link_libraries(yave-imgui PRIVATE yave::config)
target_link_libraries(yave-imgui PRIVATE yave::lib::imgui)
target_link_libraries(yave-imgui PRIVATE yave::support)
target_link_libraries(yave-imgui PRIVATE yave::node)
target_link_libraries(yave-imgui PRIVATE yave::module::std)
target_link_libraries(yave-imgui PRIVATE yave::wm)