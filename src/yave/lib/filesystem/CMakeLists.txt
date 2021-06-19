# yave::lib::filsystem
yave_add_interface_library(yave-lib-filesystem)

if(YAVE_COMPILER_GNU OR YAVE_COMPILER_LLVM)
  # <experimental/filesystem>
  target_link_libraries(yave-lib-filesystem INTERFACE stdc++fs)
endif()