# yave::lib::glfw

add_library(yave-lib-glfw glfw_context.cpp)
add_library(yave::lib::glfw ALIAS yave-lib-glfw)

target_link_libraries(yave-lib-glfw PRIVATE yave::config)
target_link_libraries(yave-lib-glfw PRIVATE yave::support::log)
target_link_libraries(yave-lib-glfw PRIVATE vulkan)
target_link_libraries(yave-lib-glfw PUBLIC glfw)
target_link_libraries(yave-lib-glfw PUBLIC glm::glm)