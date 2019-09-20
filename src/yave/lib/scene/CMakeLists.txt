# yave::lib::scene

add_library(yave-lib-scene scene_config.cpp)
add_library(yave::lib::scene ALIAS yave-lib-scene)

target_include_directories(yave-lib-scene PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-lib-scene PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-lib-scene PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-lib-scene PRIVATE yave-support-log)
target_link_libraries(yave-lib-scene PRIVATE yave-lib-image)