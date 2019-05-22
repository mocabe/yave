# yave::core::primitive

add_library(yave-core-primitive 
  primitive.cpp)
add_library(yave::core::primitive ALIAS yave-core-primitive)

target_include_directories(yave-core-primitive PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-core-primitive PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-core-primitive PRIVATE ${YAVE_LINK_FLAGS})