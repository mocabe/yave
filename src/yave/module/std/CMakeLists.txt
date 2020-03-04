# yave::module::std

add_library(yave-module-std
  module.cpp
)

add_library(yave::module::std ALIAS yave-module-std)

target_link_libraries(yave-module-std PUBLIC yave::config)
target_link_libraries(yave-module-std PUBLIC yave::node::core)
target_link_libraries(yave-module-std PRIVATE yave::support::log)
target_link_libraries(yave-module-std PRIVATE yave::support::error)
target_link_libraries(yave-module-std PRIVATE yave::lib::image_buffer)
target_link_libraries(yave-module-std PRIVATE yave::lib::scene)