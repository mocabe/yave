# yave::node::parser

add_library(yave-node-parser
  node_parser.cpp
  errors.cpp)

add_library(yave::node::parser ALIAS yave-node-parser)

target_include_directories(yave-node-parser PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-node-parser PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-node-parser PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-node-parser PRIVATE yave-core-rts yave-node-core yave-node-support yave-support-log yave-support-error)
target_link_libraries(yave-node-parser PRIVATE fmt::fmt)