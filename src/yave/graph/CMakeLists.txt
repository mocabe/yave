add_library(yave-graph INTERFACE)
add_library(yave::graph ALIAS yave-graph)
target_include_directories(yave-graph INTERFACE ${YAVE_INCLUDE_DIR})
target_link_libraries(yave-graph INTERFACE yave::tools::uid)