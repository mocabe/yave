# yave::lib::glfw

yave_add_library(yave-lib-glfw glfw_context.cpp)
 
target_link_libraries(yave-lib-glfw PRIVATE yave::core)
target_link_libraries(yave-lib-glfw PRIVATE vulkan)
target_link_libraries(yave-lib-glfw PUBLIC glfw)
target_link_libraries(yave-lib-glfw PUBLIC glm::glm)