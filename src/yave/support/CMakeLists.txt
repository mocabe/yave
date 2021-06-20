# yave::support::error
yave_add_library(yave-support-error error.cpp)

target_link_libraries(yave-support-error PRIVATE yave::config)

# yave::support
yave_add_interface_library(yave-support)

target_link_libraries(yave-support INTERFACE yave::support::error)