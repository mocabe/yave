add_subdirectory(core)

# yave::module::std

add_library(yave-module-std INTERFACE)
add_library(yave::module::std ALIAS yave-module-std)

target_link_libraries(yave-module-std INTERFACE yave::module::std::core)