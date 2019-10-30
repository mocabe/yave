# freetype
find_package(Freetype)

if(NOT Freetype_FOUND)
  init_submodule(freetype2)
  build_submodule(freetype2)
  find_package(Freetype REQUIRED)
endif()