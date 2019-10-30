# libjpeg
find_package(JPEG REQUIRED)

if(NOT JPEG_FOUND)
  init_submodule(libjpeg)
  build_submodule(libjpeg)
  find_package(JPEG REQUIRED)
endif()