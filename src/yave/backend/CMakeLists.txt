add_subdirectory(default)

# yave::backend

add_library(yave-backend backend.cpp)
add_library(yave::backend ALIAS yave-backend)

target_include_directories(yave-backend PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-backend PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-backend PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-backend PRIVATE yave::support::log)
target_link_libraries(yave-backend PRIVATE yave::node::core)