add_subdirectory(rts)
add_subdirectory(image)
add_subdirectory(frame_buffer)
add_subdirectory(primitive)

add_library(yave-core INTERFACE)
add_library(yave::core ALIAS yave-core)

target_link_libraries(yave-core INTERFACE yave-core-rts)
target_link_libraries(yave-core INTERFACE yave-core-image)
target_link_libraries(yave-core INTERFACE yave-core-frame_buffer)
target_link_libraries(yave-core INTERFACE yave-core-primitive)