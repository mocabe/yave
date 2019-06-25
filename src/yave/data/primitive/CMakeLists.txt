# yave::data::primitive

add_library(yave-data-primitive 
  primitive.cpp)
add_library(yave::data::primitive ALIAS yave-data-primitive)

target_include_directories(yave-data-primitive PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-data-primitive PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-data-primitive PRIVATE ${YAVE_LINK_FLAGS})