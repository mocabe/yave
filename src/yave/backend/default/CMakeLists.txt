
add_subdirectory(render)

# yave-backend-default

add_library(yave-backend-default INTERFACE)
add_library(yave::backend::default ALIAS yave-backend-default)
target_link_libraries(yave-backend-default INTERFACE yave::backend::default::render)