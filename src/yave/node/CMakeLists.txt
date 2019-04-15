add_library(yave-node 
  bind_info.cpp
  edge_property.cpp
  node_property.cpp
  socket_property.cpp)

  target_include_directories(yave-node PRIVATE ${YAVE_INCLUDE_DIR})
  target_compile_options(yave-node PRIVATE ${YAVE_COMPILE_FLAGS})
  target_compile_features(yave-node PRIVATE cxx_std_17)
  target_link_options(yave-node PRIVATE ${YAVE_LINK_FLAGS})