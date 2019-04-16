add_library(yave-rts 
  dynamic_typing.cpp
  eval.cpp
)

target_include_directories(yave-rts PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-rts PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-rts PRIVATE ${YAVE_LINK_FLAGS})