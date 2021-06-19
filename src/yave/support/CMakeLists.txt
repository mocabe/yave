# yave::support::log
yave_add_library(yave-support-log log.cpp)
 
target_link_libraries(yave-support-log PRIVATE yave::config)
target_link_libraries(yave-support-log PUBLIC spdlog::spdlog)

# yave::support::id
yave_add_library(yave-support-id id.cpp)
 
target_link_libraries(yave-support-id PRIVATE yave::config)
target_link_libraries(yave-support-id PRIVATE Boost::random)

# yave::support::uuid
yave_add_library(yave-support-uuid uuid.cpp)
 
target_link_libraries(yave-support-uuid PRIVATE yave::config)
target_link_libraries(yave-support-uuid PRIVATE Boost::random)

if(WIN32)
  # BCryptGenRandom, etc.
  target_link_libraries(yave-support-uuid PRIVATE Bcrypt)
endif()

# yave::support::error
yave_add_library(yave-support-error error.cpp)

target_link_libraries(yave-support-error PRIVATE yave::config)

# yave::support
yave_add_interface_library(yave-support)

target_link_libraries(yave-support INTERFACE yave::support::log)
target_link_libraries(yave-support INTERFACE yave::support::id)
target_link_libraries(yave-support INTERFACE yave::support::uuid)
target_link_libraries(yave-support INTERFACE yave::support::error)