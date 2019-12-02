# yave::lib::image

add_library(yave-lib-image 
  image.cpp
  image_view.cpp
  image_format.cpp
  image_blend.cpp
  image_io.cpp
)
add_library(yave::lib::image ALIAS yave-lib-image)

target_link_libraries(yave-lib-image PRIVATE yave::config)
target_link_libraries(yave-lib-image PRIVATE yave::support::log)
target_link_libraries(yave-lib-image PRIVATE selene)
target_link_libraries(yave-lib-image PRIVATE Boost::filesystem)

# for boost::dll
if(UNIX)
  target_link_libraries(yave-lib-image PRIVATE -ldl)
endif()