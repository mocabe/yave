add_subdirectory(default)

# yave::backend

add_library(yave-backend INTERFACE)
add_library(yave::backend ALIAS yave-backend)

target_link_libraries(yave-backend INTERFACE yave::backend::default)