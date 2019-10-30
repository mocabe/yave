# libpng
find_package(PNG REQUIRED)

if(NOT PNG_FOUND)
  init_submodule(libpng)
  build_submodule(libpng)
  find_package(PNG REQUIRED)
endif()