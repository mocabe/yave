# yave-glfw

add_library(yave-glfw glfw_context.cpp)
add_library(yave::glfw ALIAS yave-glfw)

target_include_directories(yave-glfw PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-glfw PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-glfw PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-glfw PRIVATE yave-support-log)

target_link_libraries(yave-glfw PUBLIC glfw)