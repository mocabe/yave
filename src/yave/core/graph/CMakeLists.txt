# yave::core::graph

add_library(yave-core-graph INTERFACE)
add_library(yave::core::graph ALIAS yave-core-graph)

target_include_directories(yave-core-graph INTERFACE ${YAVE_INCLUDE_DIR})
target_link_libraries(yave-core-graph INTERFACE yave::support::id)