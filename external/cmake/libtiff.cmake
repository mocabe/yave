# libtiff
#find_package(TIFF REQUIRED)

if(NOT TIFF_FOUND) 
  init_submodule(libtiff)
  build_submodule(libtiff)
  find_package(TIFF REQUIRED)
endif()