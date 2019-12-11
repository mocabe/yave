# yave::backend::default::common

add_library(yave-backend-default-common
  backend.cpp
)
add_library(yave::backend::default::common ALIAS yave-backend-default-common)

target_link_libraries(yave-backend-default-common PRIVATE yave::config)
target_link_libraries(yave-backend-default-common PRIVATE yave::node)
target_link_libraries(yave-backend-default-common PRIVATE yave::support::log)