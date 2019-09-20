# yave::lib::frame_buffer

add_library(yave-lib-frame_buffer
  frame_buffer.cpp
  frame_buffer_manager.cpp
)

add_library(yave::lib::frame_buffer ALIAS yave-lib-frame_buffer)

target_include_directories(yave-lib-frame_buffer PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-lib-frame_buffer PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-lib-frame_buffer PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-lib-frame_buffer PUBLIC yave::lib::image)
target_link_libraries(yave-lib-frame_buffer PUBLIC yave::lib::buffer)
target_link_libraries(yave-lib-frame_buffer PUBLIC yave::support::id)