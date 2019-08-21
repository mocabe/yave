# yave::lib::image

add_library(yave-lib-image 
  image.cpp
  image_view.cpp
  image_format.cpp
  image_blend.cpp
)
add_library(yave::lib::image ALIAS yave-lib-image)

target_include_directories(yave-lib-image PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-lib-image PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-lib-image PRIVATE ${YAVE_LINK_FLAGS})

target_link_libraries(yave-lib-image PRIVATE selene)