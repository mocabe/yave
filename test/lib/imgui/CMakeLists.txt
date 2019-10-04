
# demo.cpp
add_executable(yave-imgui-demo demo.cpp)
target_include_directories(yave-imgui-demo PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui-demo PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui-demo PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui-demo PRIVATE yave::lib::imgui)

# button.cpp
add_executable(yave-imgui-button button.cpp)
target_include_directories(yave-imgui-button PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui-button PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui-button PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui-button PRIVATE yave::lib::imgui)

# image.cpp
add_executable(yave-imgui-image image.cpp)
target_include_directories(yave-imgui-image PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui-image PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui-image PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui-image PRIVATE yave::lib::imgui)
target_link_libraries(yave-imgui-image PRIVATE selene)

# canvas.cpp
add_executable(yave-imgui-canvas canvas.cpp)
target_include_directories(yave-imgui-canvas PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui-canvas PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui-canvas PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui-canvas PRIVATE yave::lib::imgui)
target_link_libraries(yave-imgui-canvas PRIVATE selene)