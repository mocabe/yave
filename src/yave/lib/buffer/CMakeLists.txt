# yave::lib::buffer

add_library(yave-lib-buffer
  buffer_manager.cpp
)

add_library(yave::lib::buffer ALIAS yave-lib-buffer)

target_include_directories(yave-lib-buffer PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-lib-buffer PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-lib-buffer PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-lib-buffer PUBLIC yave::support::id)
target_link_libraries(yave-lib-buffer PUBLIC yave::support::log)