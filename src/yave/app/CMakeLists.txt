# yave::app

add_library(yave-app
  node_data_thread.cpp
  node_compiler_thread.cpp
  editor_context.cpp
)

add_library(yave::app ALIAS yave-app)

target_link_libraries(yave-app PRIVATE yave::config)
target_link_libraries(yave-app PRIVATE yave::node::core)
target_link_libraries(yave-app PRIVATE yave::support::log)
target_link_libraries(yave-app PRIVATE yave::support::id)