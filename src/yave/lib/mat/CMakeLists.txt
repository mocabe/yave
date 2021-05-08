# yave::lib::mat

add_library(yave-lib-mat INTERFACE)
add_library(yave::lib::mat ALIAS yave-lib-mat)

target_link_libraries(yave-lib-mat INTERFACE glm::glm)