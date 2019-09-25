# yave::backend::default::render

add_library(yave-backend-default-render
  system.cpp
)
add_library(yave::backend::default::render ALIAS yave-backend-default-render)

target_include_directories(yave-backend-default-render PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-backend-default-render PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-backend-default-render PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-backend-default-render PRIVATE yave::node)
target_link_libraries(yave-backend-default-render PRIVATE yave::support::log)