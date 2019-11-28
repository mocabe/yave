# yave::editor::imgui

add_library(yave-editor-imgui 
  editor_context.cpp
  node_view.cpp
  socket_view.cpp
  input_state.cpp
  style.cpp
)

add_library(yave::editor::imgui ALIAS yave-editor-imgui)

target_link_libraries(yave-editor-imgui PRIVATE yave::config)
target_link_libraries(yave-editor-imgui PRIVATE yave::node::core)
target_link_libraries(yave-editor-imgui PRIVATE yave::support::id)
target_link_libraries(yave-editor-imgui PRIVATE yave::support::log)
target_link_libraries(yave-editor-imgui PRIVATE yave::lib::glfw)
target_link_libraries(yave-editor-imgui PRIVATE yave::lib::vulkan)
target_link_libraries(yave-editor-imgui PRIVATE yave::lib::imgui)