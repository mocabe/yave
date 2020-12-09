# yave::editor

add_library(yave-editor
  data_command.cpp
  data_context.cpp
  view_command.cpp
  view_context.cpp
  compile_thread.cpp
  execute_thread.cpp
  update_channel.cpp
  editor_data.cpp
)

add_library(yave::editor ALIAS yave-editor)

target_link_libraries(yave-editor PUBLIC yave::config)
target_link_libraries(yave-editor PUBLIC yave::node::core)
target_link_libraries(yave-editor PUBLIC yave::compiler)
target_link_libraries(yave-editor PUBLIC yave::lib::scene)
target_link_libraries(yave-editor PRIVATE yave::support::log)
target_link_libraries(yave-editor PRIVATE yave::support::error)
target_link_libraries(yave-editor PRIVATE cereal)