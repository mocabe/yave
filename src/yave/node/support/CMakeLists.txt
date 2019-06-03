# yave::node::support

add_library(yave-node-support
  node_info_manager.cpp
  bind_info_manager.cpp
  socket_instance_manager.cpp)

add_library(yave::node::support ALIAS yave-node-support)

target_include_directories(yave-node-support PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-node-support PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-node-support PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-node-support PRIVATE yave::node::core)
target_link_libraries(yave-node-support PRIVATE yave::support::log)

