# yave::editor

add_library(yave-editor
  data_command.cpp
  data_context.cpp
  view_command.cpp
  view_context.cpp
  render_context.cpp
)

add_library(yave::editor ALIAS yave-editor)

target_link_libraries(yave-editor PUBLIC yave::config)
target_link_libraries(yave-editor PUBLIC yave::node::core)
target_link_libraries(yave-editor PUBLIC yave::lib::glfw)
target_link_libraries(yave-editor PRIVATE yave::lib::vulkan)
target_link_libraries(yave-editor PRIVATE yave::support::log)
target_link_libraries(yave-editor PRIVATE yave::wm)