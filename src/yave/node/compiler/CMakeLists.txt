# yave::node::compiler

add_library(yave-node-compiler
  node_compiler.cpp
  executable.cpp
  errors.cpp)

add_library(yave::node::compiler ALIAS yave-node-compiler)

target_link_libraries(yave-node-compiler PRIVATE yave::config)
target_link_libraries(yave-node-compiler PRIVATE yave::node::core)
target_link_libraries(yave-node-compiler PRIVATE yave::support::log)
target_link_libraries(yave-node-compiler PRIVATE yave::support::error)