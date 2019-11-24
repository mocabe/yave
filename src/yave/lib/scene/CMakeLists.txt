# yave::lib::scene

add_library(yave-lib-scene scene_config.cpp)
add_library(yave::lib::scene ALIAS yave-lib-scene)

target_link_libraries(yave-lib-scene PRIVATE yave::config)
target_link_libraries(yave-lib-scene PRIVATE yave::support::log)
target_link_libraries(yave-lib-scene PRIVATE yave::lib::image)