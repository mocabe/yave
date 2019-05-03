add_library(yave-image 
  image.cpp
)

target_include_directories(yave-image PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-image PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-image PRIVATE ${YAVE_LINK_FLAGS})