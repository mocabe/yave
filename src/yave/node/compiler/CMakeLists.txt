# yave::node::compiler

add_library(yave-node-compiler
  node_compiler.cpp
  executable.cpp)

add_library(yave::node::compiler ALIAS yave-node-compiler)

target_include_directories(yave-node-compiler PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-node-compiler PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-node-compiler PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-node-compiler PRIVATE yave-core-rts yave-node-core yave-node-support yave-support-log yave-support-error)
target_link_libraries(yave-node-compiler PRIVATE fmt::fmt)