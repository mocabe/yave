# yave::node::core

add_library(yave-node-core 
  primitive.cpp
  node_info.cpp
  socket_info.cpp
  connection_info.cpp
  edge_property.cpp
  node_property.cpp
  socket_property.cpp
  node_graph.cpp
  managed_node_info.cpp
  managed_socket_info.cpp
  managed_connection_info.cpp
  managed_node_graph.cpp
  socket_instance_manager.cpp
  shared_node_handle.cpp
  node_initializer.cpp
  node_declaration_store.cpp
  node_definition_store.cpp
)

add_library(yave::node::core ALIAS yave-node-core)

target_link_libraries(yave-node-core PRIVATE yave::config)
target_link_libraries(yave-node-core PRIVATE yave::support::log)
target_link_libraries(yave-node-core PRIVATE yave::support::id)