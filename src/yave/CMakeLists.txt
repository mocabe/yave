add_subdirectory(node)
add_subdirectory(backend)
add_subdirectory(support)
add_subdirectory(data)
add_subdirectory(lib)

# yave::all

add_library(yave-all INTERFACE)
add_library(yave::all ALIAS yave-all)

target_link_libraries(yave-all INTERFACE yave-node)
target_link_libraries(yave-all INTERFACE yave-backend)
target_link_libraries(yave-all INTERFACE yave-support)
target_link_libraries(yave-all INTERFACE yave-glfw)
target_link_libraries(yave-all INTERFACE yave-vulkan)
target_link_libraries(yave-all INTERFACE yave-imgui)