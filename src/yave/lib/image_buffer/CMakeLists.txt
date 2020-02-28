# yave::lib::image_buffer

add_library(yave-lib-image_buffer
  image_buffer_manager.cpp
)

add_library(yave::lib::image_buffer ALIAS yave-lib-image_buffer)

target_link_libraries(yave-lib-image_buffer PRIVATE yave::config)
target_link_libraries(yave-lib-image_buffer PRIVATE yave::support::id)
target_link_libraries(yave-lib-image_buffer PRIVATE yave::support::uuid)
target_link_libraries(yave-lib-image_buffer PRIVATE yave::support::log)
target_link_libraries(yave-lib-image_buffer PRIVATE yave::lib::image)