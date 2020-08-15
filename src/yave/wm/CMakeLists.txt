# yave::wm

add_library(yave-wm
  root_window.cpp
  window_manager.cpp
  window.cpp
  window_traverser.cpp
  event_dispatcher.cpp
  key_events.cpp
  mouse_events.cpp
  system_events.cpp
  viewport_io.cpp
  mouse_event_emitter.cpp
  key_event_emitter.cpp
  viewport.cpp
)

add_library(yave::wm ALIAS yave-wm)

target_link_libraries(yave-wm PUBLIC yave::config)
target_link_libraries(yave-wm PUBLIC yave::lib::vec)
target_link_libraries(yave-wm PUBLIC yave::lib::glfw)
target_link_libraries(yave-wm PUBLIC yave::lib::vulkan)
target_link_libraries(yave-wm PRIVATE yave::support::id)
target_link_libraries(yave-wm PRIVATE yave::support::log)
target_link_libraries(yave-wm PRIVATE yave::editor)
target_link_libraries(yave-wm PRIVATE tl::optional)