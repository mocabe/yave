add_subdirectory(frame_buffer)
add_subdirectory(image)

add_library(yave-data INTERFACE)
add_library(yave::data ALIAS yave-data)

target_link_libraries(yave-data INTERFACE yave-data-image)
target_link_libraries(yave-data INTERFACE yave-data-frame_buffer)