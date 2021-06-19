# demo.cpp
add_executable(yave-imgui-demo demo.cpp)
target_link_libraries(yave-imgui-demo PRIVATE yave::config)
target_link_libraries(yave-imgui-demo PRIVATE yave::lib::imgui)
set_target_properties(yave-imgui-demo PROPERTIES FOLDER "yave-imgui")

# button.cpp
add_executable(yave-imgui-button button.cpp)
target_link_libraries(yave-imgui-button PRIVATE yave::config)
target_link_libraries(yave-imgui-button PRIVATE yave::lib::imgui)
set_target_properties(yave-imgui-button PROPERTIES FOLDER "yave-imgui")

# image.cpp
add_executable(yave-imgui-image image.cpp)
target_link_libraries(yave-imgui-image PRIVATE yave::config)
target_link_libraries(yave-imgui-image PRIVATE yave::lib::imgui)
set_target_properties(yave-imgui-image PROPERTIES FOLDER "yave-imgui")

# canvas.cpp
add_executable(yave-imgui-canvas canvas.cpp)
target_link_libraries(yave-imgui-canvas PRIVATE yave::config)
target_link_libraries(yave-imgui-canvas PRIVATE yave::lib::imgui)
set_target_properties(yave-imgui-canvas PROPERTIES FOLDER "yave-imgui")