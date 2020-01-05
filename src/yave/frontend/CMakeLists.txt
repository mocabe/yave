# yave::frontend

add_library(yave-frontend
  node_data_thread.cpp
)

add_library(yave::frontend ALIAS yave-frontend)

target_link_libraries(yave-frontend PRIVATE yave::config)
target_link_libraries(yave-frontend PRIVATE yave::node::core)
target_link_libraries(yave-frontend PRIVATE yave::support::log)
target_link_libraries(yave-frontend PRIVATE yave::support::id)