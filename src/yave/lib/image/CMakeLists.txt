# yave::lib::image

yave_add_library(yave-lib-image 
  image.cpp
  image_view.cpp
  image_format.cpp
)

target_link_libraries(yave-lib-image PRIVATE yave::core)
target_link_libraries(yave-lib-image PRIVATE Boost::filesystem)
target_link_libraries(yave-lib-image PRIVATE fmt::fmt)

# for boost::dll
if(UNIX)
  target_link_libraries(yave-lib-image PRIVATE -ldl)
endif()