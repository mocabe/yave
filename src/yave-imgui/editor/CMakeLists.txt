# yave::editor::imgui

add_library(yave-editor-imgui 
  canvas.cpp
)

add_library(yave::editor::imgui ALIAS yave-editor-imgui)

target_link_libraries(yave-editor-imgui PRIVATE yave::config)
target_link_libraries(yave-editor-imgui PRIVATE yave::lib::imgui)
target_link_libraries(yave-editor-imgui PRIVATE yave::app)
target_link_libraries(yave-editor-imgui PRIVATE yave::support::log)
