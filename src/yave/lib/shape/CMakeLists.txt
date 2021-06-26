# yave::lib::shape

yave_add_library(yave-lib-shape 
  path.cpp
  shape.cpp
  circle.cpp
  rect.cpp
  draw.cpp
)

target_link_libraries(yave-lib-shape PRIVATE yave::core)
target_link_libraries(yave-lib-shape PRIVATE yave::lib::mat)
target_link_libraries(yave-lib-shape PRIVATE yave::lib::image)
target_link_libraries(yave-lib-shape PRIVATE blend2d)
target_link_libraries(yave-lib-shape PRIVATE glm::glm)