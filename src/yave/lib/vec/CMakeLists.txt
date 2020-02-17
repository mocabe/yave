# yave::lib::vec

add_library(yave-lib-vec INTERFACE)
add_library(yave::lib::vec ALIAS yave-lib-vec)

target_link_libraries(yave-lib-vec INTERFACE glm)