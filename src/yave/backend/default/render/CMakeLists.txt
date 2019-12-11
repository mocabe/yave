# yave::backend::default::render

add_library(yave-backend-default-render
  backend.cpp
)
add_library(yave::backend::default::render ALIAS yave-backend-default-render)

target_link_libraries(yave-backend-default-render PRIVATE yave::config)
target_link_libraries(yave-backend-default-render PRIVATE yave::node)
target_link_libraries(yave-backend-default-render PRIVATE yave::support::log)