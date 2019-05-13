add_library(yave-frame_buffer
  frame_buffer.cpp
  frame_buffer_manager.cpp)

add_library(yave::image ALIAS yave-frame_buffer)

target_include_directories(yave-frame_buffer PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-frame_buffer PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-frame_buffer PRIVATE ${YAVE_LINK_FLAGS})