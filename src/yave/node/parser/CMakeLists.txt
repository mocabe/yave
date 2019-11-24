# yave::node::parser

add_library(yave-node-parser
  node_parser.cpp
  errors.cpp)

add_library(yave::node::parser ALIAS yave-node-parser)

target_link_libraries(yave-node-parser PRIVATE yave::config)
target_link_libraries(yave-node-parser PRIVATE yave::node::core)
target_link_libraries(yave-node-parser PRIVATE yave::support::log)
target_link_libraries(yave-node-parser PRIVATE yave::support::error)
target_link_libraries(yave-node-parser PRIVATE fmt::fmt)