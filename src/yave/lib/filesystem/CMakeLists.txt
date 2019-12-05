# yave::lib::filsystem

add_library(yave-lib-filesystem INTERFACE)

add_library(yave::lib::filesystem ALIAS yave-lib-filesystem)

if(YAVE_COMPILER_GNU)
  # <experimental/filesystem>
  target_link_libraries(yave-lib-filesystem INTERFACE stdc++fs)
endif()