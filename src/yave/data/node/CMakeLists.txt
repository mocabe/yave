# yave::data::node

yave_add_library(yave-data-node
  argument.cpp
)

target_link_libraries(yave-data-node PRIVATE yave::config)