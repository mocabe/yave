add_library(yave-node 
  exception.cpp
  primitive.cpp
  bind_info.cpp
  bind_info_manager.cpp
  node_info.cpp
  node_info_manager.cpp
  socket_instance_manager.cpp
  edge_property.cpp
  node_property.cpp
  socket_property.cpp
  connection_info.cpp
  node_graph.cpp
)

  target_include_directories(yave-node PRIVATE ${YAVE_INCLUDE_DIR})
  target_compile_options(yave-node PRIVATE ${YAVE_COMPILE_FLAGS})
  target_link_options(yave-node PRIVATE ${YAVE_LINK_FLAGS})