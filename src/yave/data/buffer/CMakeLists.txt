# yave::data::buffer

add_library(yave-data-buffer
  buffer_manager.cpp
)

add_library(yave::data::buffer ALIAS yave-data-buffer)

target_link_libraries(yave-data-buffer PRIVATE yave::config)
target_link_libraries(yave-data-buffer PRIVATE yave::support::id)
target_link_libraries(yave-data-buffer PRIVATE yave::support::log)