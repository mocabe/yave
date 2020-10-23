# yave::data::frame_buffer

add_library(yave-data-frame_buffer
  frame_buffer_manager.cpp
)

add_library(yave::data::frame_buffer ALIAS yave-data-frame_buffer)

target_link_libraries(yave-data-frame_buffer PRIVATE yave::config)
target_link_libraries(yave-data-frame_buffer PRIVATE yave::lib::image)
target_link_libraries(yave-data-frame_buffer PRIVATE yave::data::buffer)
target_link_libraries(yave-data-frame_buffer PRIVATE yave::lib::vulkan)
target_link_libraries(yave-data-frame_buffer PRIVATE yave::support::id)
target_link_libraries(yave-data-frame_buffer PRIVATE yave::support::log)