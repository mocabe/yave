# yave::lib::image

add_library(yave-lib-image 
  image.cpp
  image_view.cpp
  image_format.cpp
)
add_library(yave::lib::image ALIAS yave-lib-image)

target_link_libraries(yave-lib-image PUBLIC yave::config)
target_link_libraries(yave-lib-image PRIVATE yave::support::log)
target_link_libraries(yave-lib-image PRIVATE Boost::filesystem)
target_link_libraries(yave-lib-image PRIVATE fmt::fmt)

# for boost::dll
if(UNIX)
  target_link_libraries(yave-lib-image PRIVATE -ldl)
endif()