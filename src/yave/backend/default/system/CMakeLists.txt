# yave::backend::default::system

add_library(yave-backend-default-system
  frame_buffer_manager.cpp)

add_library(yave::backend::default::system ALIAS yave-backend-default-system)

target_include_directories(yave-backend-default-system PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-backend-default-system PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-backend-default-system PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-backend-default-system PUBLIC yave::data::frame_buffer)