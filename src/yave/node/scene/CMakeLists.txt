# yave::node::scene

add_library(yave-node-scene
  managed_node_graph.cpp
  layered_node_graph.cpp
  layer_info.cpp
)

add_library(yave::node::scene ALIAS yave-node-scene)

target_include_directories(yave-node-scene PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-node-scene PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-node-scene PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-node-scene PRIVATE yave::support::log)
target_link_libraries(yave-node-scene PRIVATE yave::support::id)
target_link_libraries(yave-node-scene PRIVATE yave::core::rts)
target_link_libraries(yave-node-scene PRIVATE yave::node::core)
target_link_libraries(yave-node-scene PRIVATE yave::node::support)