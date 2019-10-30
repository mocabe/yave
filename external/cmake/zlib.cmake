# zlib
find_package(ZLIB)

if(NOT ZLIB_FOUND)
  init_submodule(zlib)
  build_submodule(zlib)
  find_package(ZLIB REQUIRED)
endif()