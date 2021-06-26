# yave::lib::scene

yave_add_library(yave-lib-scene scene_config.cpp)
 
target_link_libraries(yave-lib-scene PRIVATE yave::core)
target_link_libraries(yave-lib-scene PRIVATE yave::lib::image)
target_link_libraries(yave-lib-scene PRIVATE yave::lib::audio)