# libpng
find_package(PNG)

if(NOT PNG_FOUND)
  init_submodule(libpng)
  build_submodule(libpng)
  find_package(PNG REQUIRED)
endif()