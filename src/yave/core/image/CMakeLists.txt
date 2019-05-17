# yave::core::image

add_library(yave-core-image 
  image.cpp
  image_view.cpp
)
add_library(yave::core::image ALIAS yave-core-image)

target_include_directories(yave-core-image PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-core-image PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-core-image PRIVATE ${YAVE_LINK_FLAGS})