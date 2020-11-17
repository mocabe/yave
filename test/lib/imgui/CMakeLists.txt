# demo.cpp
add_executable(yave-imgui-demo demo.cpp)
target_link_libraries(yave-imgui-demo PRIVATE yave::config)
target_link_libraries(yave-imgui-demo PRIVATE yave::lib::imgui)

# button.cpp
add_executable(yave-imgui-button button.cpp)
target_link_libraries(yave-imgui-button PRIVATE yave::config)
target_link_libraries(yave-imgui-button PRIVATE yave::lib::imgui)

# image.cpp
add_executable(yave-imgui-image image.cpp)
target_link_libraries(yave-imgui-image PRIVATE yave::config)
target_link_libraries(yave-imgui-image PRIVATE yave::lib::imgui)

# canvas.cpp
add_executable(yave-imgui-canvas canvas.cpp)
target_link_libraries(yave-imgui-canvas PRIVATE yave::config)
target_link_libraries(yave-imgui-canvas PRIVATE yave::lib::imgui)