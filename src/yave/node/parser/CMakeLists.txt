# yave::node::parser

add_library(yave-node-parser
  node_parser.cpp
  errors.cpp
  desugar_keyframe.cpp)

add_library(yave::node::parser ALIAS yave-node-parser)

target_include_directories(yave-node-parser PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-node-parser PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-node-parser PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-node-parser PRIVATE yave::node::core)
target_link_libraries(yave-node-parser PRIVATE yave::node::support)
target_link_libraries(yave-node-parser PRIVATE yave::support::log)
target_link_libraries(yave-node-parser PRIVATE yave::support::error)
target_link_libraries(yave-node-parser PRIVATE fmt::fmt)