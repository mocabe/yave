# yave::interface

add_library(yave-interface 
  node_tree.cpp
  error_list_wrapper.cpp
  root_manager.cpp)

add_library(yave::interface ALIAS yave-interface)

target_include_directories(yave-interface PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-interface PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-interface PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-interface PRIVATE yave::all)