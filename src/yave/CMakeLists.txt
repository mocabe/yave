add_subdirectory(core)
add_subdirectory(node)
add_subdirectory(support)

# yave::all

add_library(yave-all INTERFACE)
add_library(yave::all ALIAS yave-all)

target_link_libraries(yave-all INTERFACE yave-core)
target_link_libraries(yave-all INTERFACE yave-node)
target_link_libraries(yave-all INTERFACE yave-support)