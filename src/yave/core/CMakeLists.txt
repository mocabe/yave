add_subdirectory(rts)

add_library(yave-core INTERFACE)
add_library(yave::core ALIAS yave-core)

target_link_libraries(yave-core INTERFACE yave-core-rts)