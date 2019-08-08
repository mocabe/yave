# yave::data::frame_buffer

add_library(yave-data-frame_buffer
  frame_buffer.cpp
)

add_library(yave::data::frame_buffer ALIAS yave-data-frame_buffer)

target_include_directories(yave-data-frame_buffer PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-data-frame_buffer PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-data-frame_buffer PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-data-frame_buffer PUBLIC yave::data::image)
target_link_libraries(yave-data-frame_buffer PUBLIC yave::support::id)