# yave::editor

add_library(yave-editor
  data_command.cpp
  data_context.cpp)

add_library(yave::editor ALIAS yave-editor)

target_link_libraries(yave-editor PUBLIC yave::config)
target_link_libraries(yave-editor PUBLIC yave::node::core)
target_link_libraries(yave-editor PUBLIC yave::lib::scene)
target_link_libraries(yave-editor PRIVATE yave::support::log)