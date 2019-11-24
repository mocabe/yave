# yave::support::log

add_library(yave-support-log log.cpp)
add_library(yave::support::log ALIAS yave-support-log)

target_link_libraries(yave-support-log PRIVATE yave::config)
target_link_libraries(yave-support-log PUBLIC spdlog)

# yave::support::id

add_library(yave-support-id id.cpp)
add_library(yave::support::id ALIAS yave-support-id)

target_link_libraries(yave-support-id PRIVATE yave::config)
target_link_libraries(yave-support-id PRIVATE Boost::random)

# yave::support::uuid

add_library(yave-support-uuid uuid.cpp)
add_library(yave::support::uuid ALIAS yave-support-uuid)

target_link_libraries(yave-support-uuid PRIVATE yave::config)

# yave::support::error

add_library(yave-support-error error.cpp)
add_library(yave::support::error ALIAS yave-support-error)

target_link_libraries(yave-support-error PRIVATE yave::config)

# yave::support

add_library(yave-support INTERFACE)
add_library(yave::support ALIAS yave-support)

target_link_libraries(yave-support INTERFACE yave::support::log)
target_link_libraries(yave-support INTERFACE yave::support::id)
target_link_libraries(yave-support INTERFACE yave::support::uuid)
target_link_libraries(yave-support INTERFACE yave::support::error)