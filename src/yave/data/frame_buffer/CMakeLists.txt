# yave::data::frame_buffer

yave_add_library(yave-data-frame_buffer
  frame_buffer_manager.cpp
)

target_link_libraries(yave-data-frame_buffer PRIVATE yave::core)
target_link_libraries(yave-data-frame_buffer PRIVATE yave::lib::image)
target_link_libraries(yave-data-frame_buffer PRIVATE yave::lib::vulkan)