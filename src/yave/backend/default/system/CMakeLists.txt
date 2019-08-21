# yave::backend::default::system

add_library(yave-backend-default-system INTERFACE)
add_library(yave::backend::default::system ALIAS yave-backend-default-system)
target_link_libraries(yave-backend-default-system INTERFACE yave-frame_buffer)