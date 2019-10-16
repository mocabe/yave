# yave::node::compiler

add_library(yave-node-compiler
  node_compiler.cpp
  executable.cpp
  errors.cpp)

add_library(yave::node::compiler ALIAS yave-node-compiler)

target_include_directories(yave-node-compiler PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-node-compiler PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-node-compiler PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-node-compiler PRIVATE yave::node::core)
target_link_libraries(yave-node-compiler PRIVATE yave::support::log)
target_link_libraries(yave-node-compiler PRIVATE yave::support::error)

target_link_libraries(yave-node-compiler PRIVATE fmt::fmt)