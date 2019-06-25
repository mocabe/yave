# yave::data::image

add_library(yave-data-image 
  image.cpp
  image_view.cpp
)
add_library(yave::data::image ALIAS yave-data-image)

target_include_directories(yave-data-image PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-data-image PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-data-image PRIVATE ${YAVE_LINK_FLAGS})