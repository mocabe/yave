# yave::node::core

add_library(yave-node-core 
  basic_node_info.cpp
  basic_socket_info.cpp
  basic_connection_info.cpp
  basic_edge_property.cpp
  basic_node_property.cpp
  basic_socket_property.cpp
  basic_node_graph.cpp
  socket_instance_manager.cpp
  node_initializer.cpp
  node_declaration.cpp
  node_declaration_store.cpp
  node_definition_store.cpp
  structured_node_graph.cpp
  properties.cpp
  serialize.cpp
)

add_library(yave::node::core ALIAS yave-node-core)

target_link_libraries(yave-node-core PUBLIC yave::config)
target_link_libraries(yave-node-core PRIVATE yave::support::log)
target_link_libraries(yave-node-core PRIVATE yave::support::id)
target_link_libraries(yave-node-core PUBLIC glm)
target_link_libraries(yave-node-core PRIVATE cereal)