# yave::lib::glfw

add_library(yave-lib-glfw glfw_context.cpp)
add_library(yave::lib::glfw ALIAS yave-lib-glfw)

target_include_directories(yave-lib-glfw PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-lib-glfw PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-lib-glfw PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-lib-glfw PRIVATE yave-support-log)

target_link_libraries(yave-lib-glfw PUBLIC glfw)