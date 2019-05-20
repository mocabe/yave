# yave::node::parser

add_library(yave-node-parser
  node_parser.cpp
  errors.cpp)

add_library(yave::node::parser ALIAS yave-node-parser)

target_include_directories(yave-node-parser PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-node-parser PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-node-parser PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-node-parser PRIVATE yave-node-core yave-support-log)
target_link_libraries(yave-node-parser PRIVATE fmt::fmt)