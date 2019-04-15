add_library(yave-tools
  log.cpp)

target_include_directories(yave-tools PRIVATE ${YAVE_INCLUDE_DIR})
target_link_libraries(yave-tools PRIVATE spdlog)
target_compile_options(yave-tools PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-tools PRIVATE ${YAVE_LINK_FLAGS})