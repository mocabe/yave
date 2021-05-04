
add_library(yave-ui
  event.cpp
  controller.cpp
  window.cpp
  root.cpp
  viewport.cpp
  native_window.cpp
  window_manager.cpp
  window_event_dispatcher.cpp
  layout_context.cpp
  layout_scope.cpp
  render_context.cpp
  render_scope.cpp
  main_context.cpp
  data_context.cpp
  view_context.cpp
  glfw_context.cpp
  glfw_window.cpp
  vulkan_context.cpp
  vulkan_device.cpp
  vulkan_surface.cpp
  vulkan_allocator.cpp
  vulkan_pipelines.cpp
  texture.cpp
  staging_buffer.cpp
  render_buffer.cpp
  viewport_renderer.cpp
  draw_list.cpp
  render_layer.cpp
)

add_library(yave::ui ALIAS yave-ui)

target_link_libraries(yave-ui PRIVATE yave::config)
target_link_libraries(yave-ui PRIVATE yave::support::log)
target_link_libraries(yave-ui PRIVATE yave::lib::vulkan)
target_link_libraries(yave-ui PRIVATE glfw)