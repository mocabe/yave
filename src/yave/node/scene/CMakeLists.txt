# yave::node::scene

add_library(yave-node-scene
  layered_node_graph.cpp
  layer_info.cpp
  scene_graph.cpp
)

add_library(yave::node::scene ALIAS yave-node-scene)

target_link_libraries(yave-node-scene PRIVATE yave::config)
target_link_libraries(yave-node-scene PRIVATE yave::support::log)
target_link_libraries(yave-node-scene PRIVATE yave::support::id)
target_link_libraries(yave-node-scene PRIVATE yave::node::core)