# yave::wm

yave_add_library(yave-wm
  root_window.cpp
  window_manager.cpp
  window.cpp
)

target_link_libraries(yave-wm PRIVATE yave::core)
target_link_libraries(yave-wm PUBLIC yave::lib::glfw)
target_link_libraries(yave-wm PUBLIC yave::lib::vulkan)
target_link_libraries(yave-wm PRIVATE yave::editor)
target_link_libraries(yave-wm PUBLIC glm::glm)