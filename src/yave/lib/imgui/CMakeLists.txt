# yave::lib::imgui

add_library(yave-lib-imgui 
  imgui_context.cpp
  invisible_button_ex.cpp)

add_library(yave::lib::imgui ALIAS yave-lib-imgui)


target_link_libraries(yave-lib-imgui PUBLIC yave::config)
target_link_libraries(yave-lib-imgui PUBLIC yave::lib::filesystem)
target_link_libraries(yave-lib-imgui PUBLIC yave::lib::glfw)
target_link_libraries(yave-lib-imgui PUBLIC yave::lib::vulkan)
target_link_libraries(yave-lib-imgui PUBLIC imgui-glfw-vulkan)
target_link_libraries(yave-lib-imgui PRIVATE yave::support::log)

target_link_libraries(yave-lib-imgui PUBLIC glm)