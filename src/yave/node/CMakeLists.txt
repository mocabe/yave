add_subdirectory(core)

# yave::node

add_library(yave-node INTERFACE)
add_library(yave::node ALIAS yave-node)

target_link_libraries(yave-node INTERFACE yave-node-core)