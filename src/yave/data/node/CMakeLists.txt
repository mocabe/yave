# yave::data::node

add_library(yave-data-node
  argument.cpp
)

add_library(yave::data::node ALIAS yave-data-node)

target_link_libraries(yave-data-node PRIVATE yave::config)