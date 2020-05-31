# yave::wm

add_library(yave-wm
  window.cpp
  root_window.cpp
  viewport_window.cpp
  viewport_io.cpp
  viewport_io_state.cpp
  viewport_graphics.cpp
  window_manager.cpp
  window_visitor.cpp
  event_dispatcher.cpp
  mouse_events.cpp)

add_library(yave::wm ALIAS yave-wm)

target_link_libraries(yave-wm PUBLIC yave::config)
target_link_libraries(yave-wm PUBLIC yave::lib::vec)
target_link_libraries(yave-wm PUBLIC yave::lib::glfw)
target_link_libraries(yave-wm PUBLIC yave::lib::vulkan)
target_link_libraries(yave-wm PRIVATE yave::support::id)
target_link_libraries(yave-wm PRIVATE yave::support::log)
target_link_libraries(yave-wm PRIVATE yave::editor)
target_link_libraries(yave-wm PRIVATE tl::optional)