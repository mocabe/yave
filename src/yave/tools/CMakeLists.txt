add_library(yave-tools
  log.cpp
  id.cpp)

target_include_directories(yave-tools PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-tools PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-tools PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-tools PUBLIC spdlog)