# yave::lib::shape

add_library(yave-lib-shape 
  path.cpp
  shape.cpp
  circle.cpp
  rect.cpp
  draw.cpp
)

add_library(yave::lib::shape ALIAS yave-lib-shape)

target_link_libraries(yave-lib-shape PRIVATE yave::config)
target_link_libraries(yave-lib-shape PRIVATE yave::lib::mat)
target_link_libraries(yave-lib-shape PRIVATE yave::lib::image)
target_link_libraries(yave-lib-shape PRIVATE blend2d)
target_link_libraries(yave-lib-shape PRIVATE glm::glm)