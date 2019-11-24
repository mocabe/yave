add_subdirectory(default)

# yave::backend

add_library(yave-backend backend.cpp)
add_library(yave::backend ALIAS yave-backend)

target_link_libraries(yave-backend PRIVATE yave::config)
target_link_libraries(yave-backend PRIVATE yave::support::log)
target_link_libraries(yave-backend PRIVATE yave::node::core)