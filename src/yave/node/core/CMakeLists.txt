# yave::node::core

yave_add_library(yave-node-core 
  node_info.cpp
  socket_info.cpp
  connection_info.cpp
  edge_property.cpp
  node_property.cpp
  socket_property.cpp
  node_graph.cpp
  socket_instance_manager.cpp
  node_initializer.cpp
  node_declaration.cpp
  node_declaration_store.cpp
  node_definition_store.cpp
  structured_node_graph.cpp
  properties.cpp
  serialize.cpp
)

target_link_libraries(yave-node-core PRIVATE yave::core)
target_link_libraries(yave-node-core PUBLIC glm::glm)
target_link_libraries(yave-node-core PRIVATE cereal)