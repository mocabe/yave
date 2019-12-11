# yave::lib::frame_buffer

add_library(yave-lib-frame_buffer
  frame_buffer_manager.cpp
)

add_library(yave::lib::frame_buffer ALIAS yave-lib-frame_buffer)

target_link_libraries(yave-lib-frame_buffer PRIVATE yave::config)
target_link_libraries(yave-lib-frame_buffer PRIVATE yave::lib::image)
target_link_libraries(yave-lib-frame_buffer PRIVATE yave::lib::buffer)
target_link_libraries(yave-lib-frame_buffer PRIVATE yave::support::id)