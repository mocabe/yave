add_subdirectory(node)
add_subdirectory(tools)
add_subdirectory(rts)

add_library(yave-all INTERFACE)
target_link_libraries(yave-all INTERFACE yave-node)
target_link_libraries(yave-all INTERFACE yave-tools)
target_link_libraries(yave-all INTERFACE yave-rts)