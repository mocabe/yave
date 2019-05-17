# yave::core::frame_buffer

add_library(yave-core-frame_buffer
  frame_buffer.cpp
  frame_buffer_manager.cpp)

add_library(yave::core::frame_buffer ALIAS yave-core-frame_buffer)

target_include_directories(yave-core-frame_buffer PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-core-frame_buffer PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-core-frame_buffer PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-core-frame_buffer PUBLIC yave-core-image)