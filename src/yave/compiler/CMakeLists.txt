# yave::compiler

yave_add_library(yave-compiler
  pipeline.cpp
  message.cpp
  executable.cpp
  typecheck.cpp
  init_pipeline.cpp
  input.cpp
  parse.cpp
  sema.cpp
  verify.cpp
  optimize.cpp
)

target_link_libraries(yave-compiler PUBLIC yave::config)
target_link_libraries(yave-compiler PRIVATE yave::node)
target_link_libraries(yave-compiler PRIVATE yave::data::node)
target_link_libraries(yave-compiler PRIVATE yave::support::log)
target_link_libraries(yave-compiler PRIVATE tl::optional)