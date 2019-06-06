# yave::core::rts

add_library(yave-core-rts 
  dynamic_typing.cpp
  eval.cpp
  generalize.cpp
  type_value.cpp
)
add_library(yave::core::rts ALIAS yave-core-rts)

target_include_directories(yave-core-rts PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-core-rts PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-core-rts PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-core-rts PRIVATE yave::support::id)
target_link_libraries(yave-core-rts PRIVATE fmt)
target_link_libraries(yave-core-rts PRIVATE Boost::boost)