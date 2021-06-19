add_subdirectory(core)

# yave::node
yave_add_interface_library(yave-node)

target_link_libraries(yave-node INTERFACE yave::node::core)