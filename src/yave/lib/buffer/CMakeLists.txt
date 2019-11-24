# yave::lib::buffer

add_library(yave-lib-buffer
  buffer_manager.cpp
)

add_library(yave::lib::buffer ALIAS yave-lib-buffer)

target_link_libraries(yave-lib-buffer PRIVATE yave::config)
target_link_libraries(yave-lib-buffer PRIVATE yave::support::id)
target_link_libraries(yave-lib-buffer PRIVATE yave::support::log)