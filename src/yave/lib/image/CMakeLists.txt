# yave::image

add_library(yave-image 
  image.cpp
  image_view.cpp
  image_format.cpp
  image_blend.cpp
)
add_library(yave::image ALIAS yave-image)

target_include_directories(yave-image PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-image PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-image PRIVATE ${YAVE_LINK_FLAGS})

target_link_libraries(yave-image PRIVATE selene)