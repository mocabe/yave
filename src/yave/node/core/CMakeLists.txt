# yave::node::core

add_library(yave-node-core 
  primitive.cpp
  bind_info.cpp
  node_info.cpp
  edge_property.cpp
  node_property.cpp
  socket_property.cpp
  connection_info.cpp
  node_graph.cpp
)
add_library(yave::node::core ALIAS yave-node-core)

target_include_directories(yave-node-core PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-node-core PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-node-core PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-node-core PRIVATE yave::support::log)
target_link_libraries(yave-node-core PRIVATE yave::support::id)
target_link_libraries(yave-node-core PRIVATE yave::core::rts)