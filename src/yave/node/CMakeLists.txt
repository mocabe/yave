add_subdirectory(core)
add_subdirectory(support)
add_subdirectory(parser)
add_subdirectory(compiler)

# yave::node

add_library(yave-node INTERFACE)
add_library(yave::node ALIAS yave-node)

target_link_libraries(yave-node INTERFACE yave-node-core)
target_link_libraries(yave-node INTERFACE yave-node-support)
target_link_libraries(yave-node INTERFACE yave-node-parser)
target_link_libraries(yave-node INTERFACE yave-node-compiler)