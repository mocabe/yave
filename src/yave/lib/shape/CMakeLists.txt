# yave::lib::shape

add_library(yave-lib-shape 
  path.cpp
  shape.cpp
)

add_library(yave::lib::shape ALIAS yave-lib-shape)

target_link_libraries(yave-lib-shape PRIVATE yave::config)
target_link_libraries(yave-lib-shape PRIVATE yave::lib::vec)
target_link_libraries(yave-lib-shape PRIVATE yave::lib::mat)