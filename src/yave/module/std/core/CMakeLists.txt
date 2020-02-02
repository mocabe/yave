# yave::module::std::core

add_library(yave-module-std-core
  module_definition.cpp
)

add_library(yave::module::std::core ALIAS yave-module-std-core)

target_link_libraries(yave-module-std-core PRIVATE yave::config)
target_link_libraries(yave-module-std-core PRIVATE yave::node::core)
target_link_libraries(yave-module-std-core PRIVATE yave::support::log)
target_link_libraries(yave-module-std-core PRIVATE yave::support::error)